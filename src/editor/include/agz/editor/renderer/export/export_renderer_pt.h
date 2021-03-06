#pragma once

#include <QCheckBox>
#include <QSpinBox>

#include <agz/editor/renderer/export/export_renderer.h>
#include <agz/editor/ui/utility/color_holder.h>
#include <agz/editor/ui/utility/real_slider.h>
#include <agz/editor/ui/utility/vec_input.h>

AGZ_EDITOR_BEGIN

class ExportRendererPT : public ExportRendererWidget
{
public:

    explicit ExportRendererPT(QWidget *parent = nullptr);

    RC<tracer::ConfigGroup> to_config() const override;

    void save_asset(AssetSaver &saver) const override;

    void load_asset(AssetLoader &loader) override;

private:

    QSpinBox *min_depth_ = nullptr;
    QSpinBox *max_depth_ = nullptr;

    RealSlider *cont_prob_ = nullptr;

    QCheckBox *use_mis_ = nullptr;
    QSpinBox *spp_      = nullptr;

    QSpinBox *worker_count_   = nullptr;
    QSpinBox *task_grid_size_ = nullptr;

    QSpinBox *specular_depth_ = nullptr;
};

AGZ_EDITOR_END
