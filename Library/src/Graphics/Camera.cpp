#include "pch.h"
#include "GfxSystem.h"
//#include <Platform/AEXInput.h> // mouse position
//#include <Platform/AEXWindow.h> // window size
#include "Camera.h"
#include "../Engine/Serialization.h"

namespace clc{

	void Camera::AddToSystem(){
		clcGraphics.AddCamera(this);
	}
	void Camera::RemoveFromSystem() {
		clcGraphics.RemoveCamera(this);
	}
	void Camera::OnCreate(){
		transform = &Owner()->mTransform;
		upVec = glm::vec3(0.f, 1.f, 0.f);
	}

	void Camera::ApplyViewport()const
	{
		auto w = 16; // <---------------------------------------------------------------------------------------------- NOT THIS
		auto h = 9;
		clcGraphics.SetViewport(
			int(w * mViewportBL.x), int(h * mViewportBL.y), 
			int(w * mViewportTR.x), int(h * mViewportTR.y));

		if (mbClearViewport){
			clcGraphics.SetClearColor(Color(0.5f, 0.5f, 0.5f, 1.f));//mViewportColor);
			clcGraphics.ClearFrameBuffer();
		}
	}

	// get view matrix inverse (TRS) = s^r^t^
	glm::mat4 Camera::ViewMtx44()const {
		return glm::lookAt(glm::vec3(mOwner->GetWorldMatrix()[3]), target, upVec);
	}
	glm::mat4 Camera::InvViewMtx44()const {
		return glm::mat4(1);//transform->WorldMtx();
	}

	// get projection matrix
	glm::mat4 Camera::ProjectionMtx44()const {
		return glm::perspective(glm::radians(fov), mViewRectangle.x / mViewRectangle.y, nearPlane, farPlane);
	}

	// get model view projection matrix
	glm::mat4 Camera::ViewProjMtx44()const {
		return ProjectionMtx44() * ViewMtx44();
	}

	glm::vec2 Camera::MouseToWorld()const
	{
		return glm::vec2(0); //WindowPointToWorld(aexInput.GetMousePos());
	}

	bool Camera::MouseInViewport()const
	{
		return WindowPointInViewport(glm::vec2(0) /*aexInput.GetMousePos()*/);
	}

	glm::vec2 Camera::WindowPointToWorld(const glm::vec2& wp)const
	{
		// mouse position is centered window coordinates such that
			// -WW/2 < x < WW/2 and -WH/2 < y < WH/2
		glm::vec2 worldP = wp;

		//glm::vec2 winSz = { (float)aexWindow.GetWidth() , (float)aexWindow.GetHeight() };

		//// 1. convert to frame buffer space, where the origin is the bottom-left
		//	// 0 < x < WW and 0 < y < WH
		//worldP.x += winSz.x / 2.0f;
		//worldP.y += winSz.y / 2.0f;

		//// step 2: get viewport size in pixels with respect to the window:
		//glm::vec2 vpSz[2] = {
		//	{mViewportBL.x * winSz.x, mViewportBL.y * winSz.y},
		//	{mViewportTR.x * winSz.x, mViewportTR.y * winSz.y}
		//};

		//// 3. convert from the camera's viewport space to the NDC space. 
		//worldP -= vpSz[0];									// undo translation to vp position
		//worldP.x = worldP.x / (vpSz[1].x - vpSz[0].x);	// undo scale by vp size
		//worldP.y = worldP.y / (vpSz[1].y - vpSz[0].y);
		//worldP -= glm::vec2(0.5f, 0.5f);			// undo offset translation

		//// 4. transform the mouse position from NDC to camera space
		//worldP.x *= mViewRectangle.x;	// Scale to camera view rectangle
		//worldP.y *= mViewRectangle.y;
		//worldP = AEMtx33::RotRad(transform->GetOrientation()) * worldP;	// rotate
		//worldP += transform->GetPosition2D();

		// 5. return the mouse position
		return worldP;
	}

	glm::vec2 Camera::WorldPointToWindow(const glm::vec2& wp)const
	{
		// store value for modifications
		auto winP = wp;

		//// convert point to NDC
		//winP = ViewMtx33() * winP;
		//winP.x /= mViewRectangle.x;
		//winP.y /= mViewRectangle.y;
		//
		//// bring point to window coordinates
		//winP.x *= (float)aexWindow.GetWidth();
		//winP.y *= (float)aexWindow.GetHeight();

		// return
		return winP;
	}

	glm::vec2 Camera::WorldPointToViewport(const glm::vec2& wp)const
	{
		// store value for modifications
		auto vpP = wp;

		// convert point to NDC
		//vpP = ViewMtx33() * vpP;
		//vpP.x /= mViewRectangle.x;
		//vpP.y /= mViewRectangle.y;

		//// get viewport size in pixels with respect to the window:
		//AEVec2 winSz = { (float)aexWindow.GetWidth() , (float)aexWindow.GetHeight() };
		//AEVec2 vpSz[2] = {
		//	{mViewportBL.x * winSz.x, mViewportBL.y * winSz.y},
		//	{mViewportTR.x * winSz.x, mViewportTR.y * winSz.y}
		//};

		//// convert point to viewport
		//vpP += AEVec2(0.5f, 0.5f);				// offset to translate center of NDC to bottomleft
		//vpP.x *= (vpSz[1].x - vpSz[0].x);	// scale by viewport width and height
		//vpP.y *= (vpSz[1].y - vpSz[0].y);
		//vpP += vpSz[0];				// translate to viewport position

		// return
		return vpP;
	}

	bool Camera::WindowPointInViewport(const glm::vec2& wp) const
	{
		// get mouse position
		glm::vec2 vpP = wp;

		// get window size
		glm::vec2 winSz = { (float)16 , (float)9 };

		// step 1: convert to frame buffer space, where the origin is the bottom-left
		// 0 < x < WW and 0 < y < WH
		vpP.x += winSz.x / 2.0f;
		vpP.y += winSz.y / 2.0f;

		// step 2: get viewport size in pixels with respect to the window:
		glm::vec2 vpSz[2] = {
			{mViewportBL.x * winSz.x, mViewportBL.y * winSz.y},
			{mViewportTR.x * winSz.x, mViewportTR.y * winSz.y}
		};

		// step 3: return whether or not the mouse position is located inside the camera viewport
		return (vpSz[0].x <= vpP.x && vpP.x <= vpSz[1].x &&
				vpSz[0].y <= vpP.y && vpP.y <= vpSz[1].y);
	}
	bool Camera::WorldPointInViewport(const glm::vec2& wp) const
	{
		// store value for modifications
		auto vpP = wp;

		// convert point to NDC
		/*vpP = ViewMtx33() * vpP;
		vpP.x /= mViewRectangle.x;
		vpP.y /= mViewRectangle.y;*/

		return ( -0.5f <= vpP.x && vpP.x <= 0.5f && 
				 -0.5f <= vpP.y && vpP.y <= 0.5f);
	}

	void Camera::Serialize(nlohmann::json& j) const
	{
		IBase::Serialize(j); // type, name and enabled

		j["target"] = target;
		j["fov"] = fov;
		j["near"] = nearPlane;
		j["far"] = farPlane;
		j["drawOrder"] = mDrawOrder;
	}

	void Camera::Deserialize(const nlohmann::json& j)
	{
		IBase::Deserialize(j);

		if (j.contains("target")) j["target"].get_to(target);
		if (j.contains("fov")) fov = j["fov"].get<float>();
		if (j.contains("near")) nearPlane = j["near"].get<float>();
		if (j.contains("far")) farPlane = j["far"].get<float>();
		if (j.contains("drawOrder")) mDrawOrder = j["drawOrder"].get<int>();
	}

	Camera* Camera::Clone() { return new Camera(*this); }

	bool Camera::operator== (const Camera& cam)
	{
		if (mViewRectangle.x - cam.mViewRectangle.x > EPSILON2 || mViewRectangle.x - cam.mViewRectangle.x < -EPSILON2)
			return false;

		if (mViewRectangle.y - cam.mViewRectangle.y > EPSILON2 || mViewRectangle.y - cam.mViewRectangle.y < -EPSILON2)
			return false;

		if (mViewportBL.x - cam.mViewportBL.x > EPSILON2 || mViewportBL.x - cam.mViewportBL.x < -EPSILON2)
			return false;

		if (mViewportBL.y - cam.mViewportBL.y > EPSILON2 || mViewportBL.y - cam.mViewportBL.y < -EPSILON2)
			return false;

		if (mViewportTR.x - cam.mViewportTR.x > EPSILON2 || mViewportTR.x - cam.mViewportTR.x < -EPSILON2)
			return false;

		if (mViewportTR.y - cam.mViewportTR.y > EPSILON2 || mViewportTR.y - cam.mViewportTR.y < -EPSILON2)
			return false;

		if (mbClearViewport != cam.mbClearViewport)
			return false;

		if (mViewportColor != cam.mViewportColor)
			return false;

		if (mDrawOrder != cam.mDrawOrder)
			return false;

		return true;
	}

	bool Camera::operator!= (const Camera& cam)
	{
		return !(*this == cam);
	}

	bool Camera::Equal(const IComp& comp)
	{
		if (const Camera* cam = dynamic_cast<const Camera*>(&comp))
			return *this == *cam;
		return false;
	}
}