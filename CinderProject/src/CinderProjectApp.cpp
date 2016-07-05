/*
*
* Copyright (c) 2015, Ban the Rewind
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or
* without modification, are permitted provided that the following
* conditions are met:
*
* Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in
* the documentation and/or other materials provided with the
* distribution.
*
* Neither the name of the Ban the Rewind nor the names of its
* contributors may be used to endorse or promote products
* derived from this software without specific prior written
* permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Camera.h"
#include "cinder/params/Params.h"
#include "cinder/gl/gl.h"

#include "Cinder-LeapMotion.h"

class CinderProjectApp : public ci::app::App
{
public:
	void 					draw() override;
	void 					setup() override;
	//
	void					update() override;
private:
	Leap::Frame				mFrame;
	LeapMotion::DeviceRef	mLeap;

	ci::CameraPersp			mCamera_p;
	ci::CameraOrtho			mCamera_o;

	//
	float						mFrameRate;
	bool						mFullScreen;
	int							mHandCount;
	int							bUI_mode;
	ci::params::InterfaceGlRef	mParams;
};

using namespace ci;
using namespace ci::app;
using namespace std;

float basicHeight;

void drawGround();

void CinderProjectApp::draw()
{
	gl::viewport(getWindowSize());
	gl::clear(Colorf::ColorT(0.6f, 0.6f, 0.6f));
	// #View1 : 2D User Interface
	gl::setMatrices(mCamera_o);
	gl::color(ColorAf::ColorAT(1.0, 0.0, 0.0));

	//if UI Select mode
	if (bUI_mode) {
		gl::lineWidth(5.f);
		gl::drawStrokedRect(Rectf(0, 0, getWindowWidth(), getWindowHeight()));
		gl::lineWidth(1.f);
	}
	//else nothing

	// #View2 : 3D space
	gl::setMatrices(mCamera_p);
	gl::color(ColorAf::ColorAT(0.3f, 0.3f, 0.3f));

	drawGround();

	float headLength = 6.0f;
	float headRadius = 3.0f;
	mHandCount = 0;
	const Leap::HandList& hands = mFrame.hands();
	for (Leap::HandList::const_iterator handIter = hands.begin(); handIter != hands.end(); ++handIter)
	{
		const Leap::Hand& hand = *handIter;
		const vec3 handDir = LeapMotion::toVec3(hand.direction());
		const vec3 palmNorm = LeapMotion::toVec3(hand.palmNormal());
		const vec3 palmPos = LeapMotion::toVec3(hand.palmPosition());
		const vec3 palmVel = LeapMotion::toVec3(hand.palmVelocity());
		const vec3 sphereCenter = LeapMotion::toVec3(hand.sphereCenter());
		const float sphereRadius = hand.sphereRadius();

		mHandCount++;

		if (dot(palmNorm, vec3(0, 1, 0)) > 0)
		{
			bUI_mode = !bUI_mode;
		}
		/*
		// Hand sphere
		{
		const gl::ScopedColor scopedColor(ColorAf(Colorf::gray(0.9f), 0.5f));
		gl::enableWireframe();
		gl::drawSphere(sphereCenter, sphereRadius, 16);
		gl::disableWireframe();
		}
		*/
		/*
		// Hand plane
		{
		const gl::ScopedColor scopedColor(ColorAf(0.75f, 0.0f, 0.75f, 0.25f));
		const gl::ScopedModelMatrix scopedModelMatrix;
		gl::translate(palmPos);
		gl::rotate(quat(palmPos, handDir));
		for (float i = 0.25f; i <= 1.0f; i += 0.25f) {
		gl::drawStrokedCircle(vec2(0.0f), sphereRadius * i, 16);
		}
		}

		// Hand direction
		{
		const gl::ScopedColor scopedColor(ColorAf(1.0f, 0.0f, 1.0f, 0.5f));
		gl::drawVector(palmPos, palmPos + handDir * 30.0f, headLength, headRadius);
		}

		// Hand normal
		{
		const gl::ScopedColor scopedColor(ColorAf(0.0f, 0.0f, 1.0f, 0.5f));
		gl::drawVector(palmPos, palmPos + palmNorm * 30.0f, headLength, headRadius);
		}

		// Hand velocity
		{
		const gl::ScopedColor scopedColor(ColorAf(0.0f, 1.0f, 0.0f, 0.5f));
		gl::drawVector(palmPos, palmPos + palmVel * 0.05f, headLength, headRadius);
		}
		*/
		// Fingers
		const Leap::PointableList& pointables = hand.pointables();
		for (Leap::PointableList::const_iterator pointIter = pointables.begin(); pointIter != pointables.end(); ++pointIter) {
			const Leap::Pointable& pointable = *pointIter;
			if (pointIter == pointables.begin())
			{
			}
			// Get pointable data
			const vec3 dir = LeapMotion::toVec3(pointable.direction());
			const bool isTool = pointable.isTool();
			const float length = pointable.length();
			const vec3 tipPos = LeapMotion::toVec3(pointable.tipPosition());
			const vec3 tipVel = LeapMotion::toVec3(pointable.tipVelocity());
			const float width = pointable.width();
			const vec3 basePos = tipPos + dir * -length;

			// Draw line representing pointable's length
			{
				const gl::ScopedColor scopedColor(ColorAf::gray(0.3f));
				gl::drawLine(basePos, tipPos);
			}

			// Tip position
			{
				const gl::ScopedColor scopedColor(isTool ? Colorf(1.0f, 0.0f, 0.0f) : Colorf::black());
				const gl::ScopedModelMatrix scopedModelMatrix;
				gl::translate(tipPos);
				gl::drawStrokedCircle(vec2(0.0f), width, 16);
			}

			// Finger velocity
			{
				const gl::ScopedColor scopedColor(ColorAf(0.0f, 1.0f, 0.0f, 0.5f));
				gl::drawVector(tipPos, tipPos + tipVel * 0.05f, headLength, headRadius);
			}
		}
	}

	/*
	for (const Leap::Hand& hand : hands) {

	//point cube
	for (const Leap::Pointable& pointable : hand.pointables()) {
	vec3 dir = LeapMotion::toVec3(pointable.direction());
	float length = pointable.length();
	vec3 tipPos = LeapMotion::toVec3(pointable.tipPosition());
	//gl::drawSolidCircle(vec2(tipPos.x, tipPos.y), 5.f, 20);
	gl::color(ColorAf::ColorAT(0.3f, 0.3f, 0.3f));
	gl::drawCube(tipPos, vec3(5.0f, 5.0f, 5.0f));
	//gl::drawColorCube(tipPos, vec3(5.0f, 5.0f, 5.0f));

	// tip point normal vecotr
	// vec3 basePos	= tipPos + dir * length;
	//gl::color( ColorAf::gray( 0.8f ) );
	//gl::drawLine( basePos, tipPos );
	}

	//bone
	const Leap::Arm& arm = hand.arm();

	vec3 palm = LeapMotion::toVec3(hand.palmPosition());
	vec3 elbow = LeapMotion::toVec3(arm.elbowPosition());
	vec3 rotation = vec3(hand.direction().pitch(), hand.direction().yaw(), hand.palmNormal().roll());
	vec3 wrist = LeapMotion::toVec3(arm.wristPosition());

	gl::drawLine(elbow, wrist);

	vector<vec3> knuckles;
	const Leap::FingerList fingers = hand.fingers();
	for (Leap::FingerList::const_iterator fingerIter = fingers.begin(); fingerIter != fingers.end(); ++fingerIter) {
	const Leap::Finger& finger = *fingerIter;

	for (int32_t i = 0; i < 4; ++i) {
	const Leap::Bone& bone = finger.bone((Leap::Bone::Type)i);
	vec3 center = LeapMotion::toVec3(bone.center());
	vec3 direction = LeapMotion::toVec3(bone.direction());
	vec3 start = center - direction * bone.length() * 0.5f;
	vec3 end = center + direction * bone.length() * 0.5f;

	if (i == 0) {
	knuckles.push_back(start);
	gl::drawLine(wrist, start);
	}
	else {
	gl::drawLine(start, end);
	}
	}
	}

	if (knuckles.size() > 1) {
	for (size_t i = 1; i < knuckles.size(); ++i) {
	const vec3& v0 = knuckles.at(i - 1);
	const vec3& v1 = knuckles.at(i);
	gl::drawLine(v0, v1);
	}
	gl::drawLine(elbow, knuckles.at(0));
	}

	}
	*/
	gl::drawColorCube(vec3(0, 100, 0), vec3(20, 20, 20));

	mParams->draw();
}

void CinderProjectApp::setup()
{
	gl::enableDepthRead();
	gl::enableDepthWrite();

	//Camera
	mCamera_o = CameraOrtho(0, getWindowWidth(), getWindowHeight(), 0.0f, 0.01f, 1000.0f);
	mCamera_o.lookAt(vec3(0.f, 0.f, 10.f), vec3(0.f, 0.f, 0.f));

	mCamera_p = CameraPersp(getWindowWidth(), getWindowHeight(), 60.0f, 0.01f, 1000.0f);
	mCamera_p.lookAt(vec3(0.0f, 200.0f, 300.0f), vec3(0.0f, 50.0f, 0.0f));

	basicHeight = 100.0f;

	//leapmotion
	mLeap = LeapMotion::Device::create();

	mLeap->connectEventHandler([&](Leap::Frame frame)
	{
		mFrame = frame;
	});

	//Parameters
	mFrameRate = 0.0f;
	mFullScreen = false;
	bUI_mode = false;

	mParams = params::InterfaceGl::create("Params", ivec2(200, 105));
	mParams->addParam("Frame rate", &mFrameRate, "", true);
	mParams->addParam("Full screen", &mFullScreen).key("f");
	mParams->addParam("Handcount", &mHandCount, "", true);
	//mParams->hide();

	/*
	// Enable gesture types
	Leap::Controller* controller = mLeap->getController();
	controller->enableGesture(Leap::Gesture::Type::TYPE_CIRCLE);
	controller->enableGesture(Leap::Gesture::Type::TYPE_KEY_TAP);
	controller->enableGesture(Leap::Gesture::Type::TYPE_SCREEN_TAP);
	controller->enableGesture(Leap::Gesture::Type::TYPE_SWIPE);
	*/
}

void CinderProjectApp::update()
{
	mFrameRate = getAverageFps();

	if (mFullScreen != isFullScreen()) {
		setFullScreen(mFullScreen);
	}
}

RendererGl::Options gOptions;
//CINDER_APP( CinderProjectApp, RendererGl )
CINDER_APP(CinderProjectApp, RendererGl(gOptions.msaa(16)), [](App::Settings* settings)
{
	settings->setWindowSize(1366, 768);
})

void drawGround()
{
	for (int i = -1000; i < 1000; i += 50) {
		gl::color(ColorAf::ColorAT(0.3, 0.3, 0.3));
		gl::drawLine(vec3(-1000, 0, i), vec3(1000, 0, i));
		gl::drawLine(vec3(i, 0, -1000), vec3(i, 0, 1000));
	}
}







































