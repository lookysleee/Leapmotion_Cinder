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

	ci::CameraPersp			mCamera;

	//
	float						mFrameRate;
	bool						mFullScreen;
	ci::params::InterfaceGlRef	mParams;
};

using namespace ci;
using namespace ci::app;
using namespace std;

float basicHeight;

void drawAxis();

void CinderProjectApp::draw()
{
	gl::viewport(getWindowSize());
	gl::clear(Colorf::white());
	gl::setMatrices(mCamera);

	drawAxis();

	gl::color(ColorAf::ColorAT(0.3, 0.3, 0.3));

	//gl::drawSolidCircle(vec2(0, 0), 100, 20);
	//gl::drawSphere(vec3(0, 0, 0), 100, -1); //���ɸ�...


	gl::rotate(45, 0, 1, 0);
	const Leap::HandList& hands = mFrame.hands();
	for (const Leap::Hand& hand : hands) {

		//point cube
		for (const Leap::Pointable& pointable : hand.pointables()) {
			vec3 dir = LeapMotion::toVec3(pointable.direction());
			float length = pointable.length();
			vec3 tipPos = LeapMotion::toVec3(pointable.tipPosition());
			gl::drawColorCube(tipPos, vec3(20.0f, 20.0f, 20.0f));

			/* tip point normal vecotr */
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

		//gl::drawLine(elbow, wrist);

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
			//gl::drawLine(elbow, knuckles.at(0));
		}
	}

	mParams->draw();
}

void CinderProjectApp::setup()
{
	gl::enableDepthRead();
	gl::enableDepthWrite();

	//Camera
	mCamera = CameraPersp(getWindowWidth(), getWindowHeight(), 60.0f, 0.01f, 1000.0f);
	mCamera.lookAt(vec3(300.0f, 200.0f, 300.0f), vec3(0.0f, 50.0f, 0.0f));

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

	mParams = params::InterfaceGl::create("Params", ivec2(200, 105));
	mParams->addParam("Frame rate", &mFrameRate, "", true);
	mParams->addParam("Full screen", &mFullScreen).key("f");
	mParams->hide();

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
	settings->setWindowSize(1024, 768);
})

void drawAxis()
{
	gl::color(ColorAf::ColorAT(1, 0, 0));
	gl::drawLine(vec3(-1000, 0, 0), vec3(1000, 0, 0));
	gl::color(ColorAf::ColorAT(0, 1, 0));
	gl::drawLine(vec3(0, -1000, 0), vec3(0, 1000, 0));
	gl::color(ColorAf::ColorAT(0, 0, 1));
	gl::drawLine(vec3(0, 0, -1000), vec3(0, 0, 1000));
}