#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include "../MatrixNode.h"
#include "../OpenGL/Program.h"
#include "Widget.h"
#include <list>

namespace CGE
{
    class UserInterface
    {
        public:
            UserInterface(float inRange = 4.0f);
            virtual ~UserInterface();

            void update();
            void display();

            void addWidget(Widget* inWidget);
            void removeWidget(Widget* inWidget);

            void onResize(int inWidth, int inHeight);
            void onMouseMove(int inX, int inY);
            void onMouseDown();
            void onMouseUp();

        protected:
        private:
            mat4f mProjection;
            float mRange;
            float mRatio;
            int mWidth;
            int mHeight;
            int mCenterX;
            int mCenterY;
            float mMouseX;
            float mMouseY;

            Widget* mMouseOverWidget;
            Widget* mClickCandidate;
            std::list<Widget*> mWidgets;

            Program<2> mProgram;

            GLuniform mUniMVPM;
            GLuniform mUniTexture;

            Matrix4x4<float> mMatrix;
            MatrixNode<float> mModelViewProjection;
    };
}

#endif
