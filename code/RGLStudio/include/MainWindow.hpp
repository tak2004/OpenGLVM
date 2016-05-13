#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <RadonFramework/Drawing/Forms/Form.hpp>
#include <RadonFramework/Drawing/Canvas3D.hpp>

namespace RGLStudio {

class MainWindow: public RF_Form::Form
{
public:
    void Initialize();

    virtual void Resize(const RF_Geo::Size2D<>& Value) override;
    virtual void Idle() override;
protected:
    RF_Mem::AutoPointer<RF_Draw::Canvas3D> m_Canvas;
};

}

#endif