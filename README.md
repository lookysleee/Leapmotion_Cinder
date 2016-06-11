# Leapmotion_Cinder

Building Virtual Reality 3D Modeling tool

1. https://libcinder.org/download 에서 Cinder 0.9.0 vc2013 을 받는다.

2. 경로는 현재 깃프로젝트인(Leapmotion_Cinder)폴더와 같은 레벨에 있어야함.
   ex) My repository\Leapmotion_cinder , My repository\cinder_0.9.0_vc2013

3. cinder_0.9.0_vc2013을 받게되면 My repository\cinder_0.9.0_vc2013\cinder_0.9.0_vc2013 에
   https://github.com/BanTheRewind/Cinder-LeapMotion 깃 프로젝트를 받는다.

4. My repository\cinder_0.9.0_vc2013\cinder_0.9.0_vc2013\Cinder-LeapMotion-master\Cinder-LeapMotion-master
  내부에는 lib, samples, src, templates, ... 등이 위치하게 된다.

5. My repository\cinder_0.9.0_vc2013\cinder_0.9.0_vc2013에서
   vc2013, vc2013_winrt 프로젝트를 각각 Build하면 되는데,
   vc2013_winrt 는 그냥 VS2015로 열어서 빌드하면 되고,
   vc2013 은 VS2015로 열 경우, version 변경확인 창을 취소하여, VS2013버전으로 빌드해야한다.

6. 다 끝나면 Leapmotion_Cinder 프로젝트 디버깅이 가능해진다.
