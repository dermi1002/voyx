#ifdef VOYXUI

#pragma once

#include <voyx/Header.h>
#include <voyx/ui/Plot.h>

#include <qcustomplot.h>

#include <QApplication>
#include <QColor>
#include <QGridLayout>
#include <QKeyEvent>
#include <QPen>
#include <QWidget>

class QPlot : public Plot
{

public:

  QPlot(const std::chrono::duration<double> delay = std::chrono::duration<double>::zero());
  ~QPlot();

  void start();
  void stop();

  void show() override;
  void show(const size_t width, const size_t height);

  void xlim(const double min, const double max) override;
  void ylim(const double min, const double max) override;

  void xrange(const double max) override;
  void xrange(const double min, const double max) override;

  void xline(const std::optional<double> x) override;
  void yline(const std::optional<double> y) override;

  void plot(const std::span<const float> y) override;
  void plot(const std::span<const double> y) override;

protected:

  void addPlot(const size_t row, const size_t col, const size_t graphs);
  QCustomPlot* getPlot(const size_t row, const size_t col) const;
  QColor getLineColor(const ptrdiff_t index) const;
  int getLineWidth(const ptrdiff_t index) const;

private:

  const std::chrono::duration<double> delay;

  std::shared_ptr<QApplication> application;
  std::shared_ptr<QWidget> widget;
  std::shared_ptr<QGridLayout> layout;

  std::vector<std::shared_ptr<QCustomPlot>> plots;
  std::map<QCustomPlot*, QCPItemStraightLine*> xlines;
  std::map<QCustomPlot*, QCPItemStraightLine*> ylines;

  struct
  {
    bool xauto = true;
    bool yauto = true;
    std::optional<std::pair<double, double>> xrange;
    std::vector<double> ydata;
    std::optional<double> xline;
    std::optional<double> yline;
  }
  data;

  std::shared_ptr<std::thread> thread;
  std::mutex mutex;
  bool doloop = false;
  bool pause = false;

  void loop();

  /*
    #!/usr/bin/env python3
    import numpy
    import seaborn
    colors = numpy.array(seaborn.color_palette())
    colors = (colors * 0xFF).clip(0, 0xFF).astype(numpy.uint8)
    colors = colors.tolist()
    print(colors)
  */

  const int colors[10][3] =
  {
    {  31, 119, 180 },
    { 255, 127,  14 },
    {  44, 160,  44 },
    { 214,  39,  40 },
    { 148, 103, 189 },
    { 140,  86,  75 },
    { 227, 119, 194 },
    { 127, 127, 127 },
    { 188, 189,  34 },
    {  23, 190, 207 },
  };

};

#endif
