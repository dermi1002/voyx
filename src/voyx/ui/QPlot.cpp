#ifdef VOYXUI

#include <voyx/ui/QPlot.h>

#include <voyx/Source.h>

namespace args
{
  char arg0[] = "voyx";
  char* argv[] = { arg0, nullptr };
  int argc = sizeof(argv) / sizeof(char*) - 1;
}

class QCloseOnEscapeWidget : public QWidget
{

private:

  void keyPressEvent(QKeyEvent* event) override
  {
    if (event->key() == Qt::Key_Escape)
    {
      close();
    }
    else
    {
      QWidget::keyPressEvent(event);
    }
  }

};

class QCustomTouchPlot : public QCustomPlot
{

public:

  void touch(std::function<void(bool state)> callback)
  {
    this->callback = callback;
  }

private:

  std::function<void(bool)> callback = [](bool state){};

  void mousePressEvent(QMouseEvent* event) override
  {
    if (event->button() == Qt::LeftButton)
    {
      callback(true);
    }
    else
    {
      QCustomPlot::mousePressEvent(event);
    }
  }

  void mouseReleaseEvent(QMouseEvent* event) override
  {
    if (event->button() == Qt::LeftButton)
    {
      callback(false);
    }
    else
    {
      QCustomPlot::mouseReleaseEvent(event);
    }
  }

};

QPlot::QPlot(const std::chrono::duration<double> delay) :
  delay(delay)
{
  application = std::make_shared<QApplication>(args::argc, args::argv);
  widget = std::make_shared<QCloseOnEscapeWidget>();

  layout = std::make_shared<QGridLayout>();
  widget->setLayout(layout.get());

  addPlot(0, 0, 1);

  start();
}

QPlot::~QPlot()
{
  stop();
}

void QPlot::start()
{
  stop();

  doloop = true;

  thread = std::make_shared<std::thread>(
    [&](){ loop(); });
}

void QPlot::stop()
{
  doloop = false;

  if (thread != nullptr)
  {
    if (thread->joinable())
    {
      thread->join();
    }

    thread = nullptr;
  }
}

void QPlot::show()
{
  show(800, 600);
}

void QPlot::show(const size_t width, const size_t height)
{
  widget->setWindowTitle(args::arg0);
  widget->resize(static_cast<int>(width), static_cast<int>(height));

  widget->show();
  application->exec();
}

void QPlot::xlim(const double min, const double max)
{
  std::lock_guard lock(mutex);
  data.xauto = false;

  for (auto plot : plots)
  {
    plot->xAxis->setRange(min, max);
  }
}

void QPlot::ylim(const double min, const double max)
{
  std::lock_guard lock(mutex);
  data.yauto = false;

  for (auto plot : plots)
  {
    plot->yAxis->setRange(min, max);
  }
}

void QPlot::xrange(const double max)
{
  std::lock_guard lock(mutex);
  data.xrange = std::pair<double, double>(0, max);
}

void QPlot::xrange(const double min, const double max)
{
  std::lock_guard lock(mutex);
  data.xrange = std::pair<double, double>(min, max);
}

void QPlot::xline(const std::optional<double> x)
{
  if (pause)
  {
    return;
  }

  std::lock_guard lock(mutex);
  data.xline = x;
}

void QPlot::yline(const std::optional<double> y)
{
  if (pause)
  {
    return;
  }

  std::lock_guard lock(mutex);
  data.yline = y;
}

void QPlot::plot(const std::span<const float> y)
{
  if (pause)
  {
    return;
  }

  std::lock_guard lock(mutex);
  data.ydata = std::vector<double>(y.begin(), y.end());
}

void QPlot::plot(const std::span<const double> y)
{
  if (pause)
  {
    return;
  }

  std::lock_guard lock(mutex);
  data.ydata = std::vector<double>(y.begin(), y.end());
}

void QPlot::addPlot(const size_t row, const size_t col, const size_t graphs)
{
  auto plot = std::make_shared<QCustomTouchPlot>();

  for (size_t i = 0; i < graphs; ++i)
  {
    plot->addGraph();

    QPen pen;

    pen.setColor(getLineColor(i));
    pen.setWidth(getLineWidth(i));

    plot->graph(i)->setPen(pen);
  }

  layout->addWidget(plot.get(), row, col);

  plot->touch([&](bool state)
  {
    pause = state;
  });

  plots.push_back(plot);

  auto xline = new QCPItemStraightLine(plot.get());
  auto yline = new QCPItemStraightLine(plot.get());
  {
    QPen pen;

    pen.setColor(getLineColor(0));
    pen.setWidth(getLineWidth(0));
    pen.setStyle(Qt::DashLine);

    xline->setPen(pen);
    yline->setPen(pen);

    xline->setVisible(false);
    yline->setVisible(false);
  }

  xlines[plot.get()] = xline;
  ylines[plot.get()] = yline;
}

QCustomPlot* QPlot::getPlot(const size_t row, const size_t col) const
{
  return dynamic_cast<QCustomPlot*>(
    layout->itemAtPosition(row, col)->widget());
}

QColor QPlot::getLineColor(const size_t index) const
{
  const auto& color = colors[index % 10];

  return QColor(color[0], color[1], color[2]);
}

int QPlot::getLineWidth(const size_t index) const
{
  return 2;
}

void QPlot::loop()
{
  const size_t row = 0;
  const size_t col = 0;
  const size_t graph = 0;

  std::array<std::chrono::milliseconds, 2> delays =
  {
    std::chrono::duration_cast<std::chrono::milliseconds>(delay),
    std::chrono::milliseconds(0)
  };

  while (doloop)
  {
    const auto delay = *std::max_element(
      delays.begin(), delays.end());

    std::this_thread::sleep_for(delay);

    bool xauto, yauto;
    std::optional<std::pair<double, double>> xrange;
    std::vector<double> ydata;
    std::optional<double> xline;
    std::optional<double> yline;
    {
      std::lock_guard lock(mutex);
      xauto = data.xauto;
      yauto = data.yauto;
      xrange = data.xrange;
      ydata = data.ydata;
      xline = data.xline;
      yline = data.yline;
    }

    std::vector<double> xdata;
    {
      xdata.resize(ydata.size());

      std::iota(xdata.begin(), xdata.end(), 0.0);

      if (xrange.has_value())
      {
        const double foo = (xrange.value().second - xrange.value().first) / xdata.size();
        const double bar = xrange.value().first;

        std::transform(xdata.begin(), xdata.end(), xdata.begin(),
          [foo, bar](double value) { return value * foo + bar; });
      }
    }

    auto plot = getPlot(row, col);
    {
      // x,y data

      const size_t size = std::min(
        xdata.size(), ydata.size());

      QVector<double> x(size), y(size);

      for (size_t i = 0; i < size; ++i)
      {
        x[i] = xdata[i];
        y[i] = ydata[i];
      }

      plot->graph(graph)->setData(x, y);

      // x,y lines

      if (xline.has_value())
      {
        xlines.at(plot)->setVisible(true);
        xlines.at(plot)->point1->setCoords(xline.value(), 0);
        xlines.at(plot)->point2->setCoords(xline.value(), 1);
      }
      else
      {
        xlines.at(plot)->setVisible(false);
      }

      if (yline.has_value())
      {
        ylines.at(plot)->setVisible(true);
        ylines.at(plot)->point1->setCoords(0, yline.value());
        ylines.at(plot)->point2->setCoords(1, yline.value());
      }
      else
      {
        ylines.at(plot)->setVisible(false);
      }

      // auto range

      if (xauto && !xdata.empty())
      {
        const double xmin = *std::min_element(xdata.begin(), xdata.end());
        const double xmax = *std::max_element(xdata.begin(), xdata.end());

        // sync again
        {
          std::lock_guard lock(mutex);

          if (data.xauto)
          {
            plot->xAxis->setRange(xmin, xmax);
          }
        }
      }

      if (yauto && !ydata.empty())
      {
        const double ymin = *std::min_element(ydata.begin(), ydata.end());
        const double ymax = *std::max_element(ydata.begin(), ydata.end());

        // sync again
        {
          std::lock_guard lock(mutex);

          if (data.yauto)
          {
            plot->yAxis->setRange(ymin, ymax);
          }
        }
      }

      // replot

      plot->replot(QCustomPlot::rpQueuedReplot);

      delays.back() = std::chrono::milliseconds(
        static_cast<int>(plot->replotTime()));
    }
  }
}

#endif
