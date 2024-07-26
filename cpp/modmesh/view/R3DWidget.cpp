/*
 * Copyright (c) 2022, Yung-Yu Chen <yyc@solvcon.net>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * - Neither the name of the copyright holder nor the names of its contributors
 *   may be used to endorse or promote products derived from this software
 *   without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <modmesh/view/R3DWidget.hpp> // Must be the first include.
#include <modmesh/view/RAxisMark.hpp>
#include <modmesh/view/RStaticMesh.hpp>

namespace modmesh
{

R3DWidget::R3DWidget(Qt3DExtras::Qt3DWindow * window, RScene * scene, QWidget * parent, Qt::WindowFlags f)
    : QWidget(parent, f)
    , m_view(nullptr == window ? new Qt3DExtras::Qt3DWindow : window)
    , m_scene(nullptr == scene ? new RScene : scene)
    , m_container(createWindowContainer(m_view, this, Qt::Widget))
{
    m_view->setRootEntity(m_scene);

    resetCamera(m_view->camera());

    if (Toggle::instance().fixed().get_show_axis())
    {
        showMark();
    }
}

void R3DWidget::showMark()
{
    for (Qt3DCore::QNode * child : m_scene->childNodes())
    {
        if (typeid(*child) == typeid(RAxisMark))
        {
            child->deleteLater();
        }
    }
    new RAxisMark(m_scene);
}

void R3DWidget::updateMesh(std::shared_ptr<StaticMesh> const & mesh)
{
    for (Qt3DCore::QNode * child : m_scene->childNodes())
    {
        if (typeid(*child) == typeid(RStaticMesh))
        {
            child->deleteLater();
        }
    }
    new RStaticMesh(mesh, m_scene);
    m_mesh = mesh;
}

void R3DWidget::updateWorld(std::shared_ptr<WorldFp64> const & world)
{
    for (Qt3DCore::QNode * child : m_scene->childNodes())
    {
        if (typeid(*child) == typeid(RWorld))
        {
            child->deleteLater();
        }
    }
    new RWorld(world, m_scene);
}

void R3DWidget::closeAndDestroy()
{
    this->close();
    this->deleteLater();
}

void R3DWidget::resizeEvent(QResizeEvent * event)
{
    QWidget::resizeEvent(event);
    m_view->resize(event->size());
    m_container->resize(event->size());
}

void R3DWidget::resetCamera(Qt3DRender::QCamera * camera,
                            float positionX,
                            float positionY,
                            float positionZ)
{
    // Set up the camera.
    camera->lens()->setPerspectiveProjection(45.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
    camera->setPosition(QVector3D(positionX, positionY, positionZ));
    camera->setViewCenter(QVector3D(0.0f, 0.0f, 0.0f));
    camera->setUpVector(QVector3D(0.f, 1.f, 0.f));

    // Set up the camera control.
    auto * control = m_scene->controller();
    control->setCamera(camera);
    control->setLinearSpeed(50.0f);
    control->setLookSpeed(180.0f);
}

} /* end namespace modmesh */

// vim: set ff=unix fenc=utf8 et sw=4 ts=4 sts=4:
