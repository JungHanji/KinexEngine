#pragma once
#include "Camera.hpp"
#include "Input.hpp"
#include "Time.hpp"

namespace knx{
    class CameraController{
        Camera *camera;
        Input *input;
        Time *time;
        public:

        void update(){
            if(input->isWindowInFocus()){
                camera->rotate(-input->getCursorAxisX(true), -input->getCursorAxisY(true));

                camera->move({
                    -(int)input->getAxisX(),
                    (int)input->getAxisY(),
                    (int)input->getAxisZ()
                }, time->getDeltaTime());
            }
        }

        CameraController(Camera *camera, Input *input, Time *time): camera(camera), input(input), time(time) {
            ;
        }

        CameraController(){}
        ~CameraController(){}
    };
};