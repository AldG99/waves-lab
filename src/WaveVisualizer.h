#ifndef WAVE_VISUALIZER_H
#define WAVE_VISUALIZER_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QFrame>
#include <QtGui/QPainter>
#include <QtGui/QPen>
#include <QtGui/QMouseEvent>
#include <QtGui/QWheelEvent>
#include <QtCore/QTimer>
#include <vector>
#include "WaveEngine.h"

enum class VisualizationMode {
    TIME_DOMAIN,
    FREQUENCY_DOMAIN,
    SUPERPOSITION,
    INTERFERENCE_PATTERN
};

class WaveVisualizer : public QWidget {

public:
    explicit WaveVisualizer(QWidget *parent = nullptr);
    ~WaveVisualizer();
    
    void setWaveEngine(WaveEngine* engine);
    void setVisualizationMode(VisualizationMode mode);
    void setTimeRange(double startTime, double endTime);
    void setFrequencyRange(double startFreq, double endFreq);
    void setCurrentTime(double time);
    void updateVisualization();
    
    // Display properties
    void setShowGrid(bool show) { m_showGrid = show; update(); }
    void setShowLegend(bool show) { m_showLegend = show; update(); }
    void setShowAxes(bool show) { m_showAxes = show; update(); }
    void setAutoScale(bool enable) { m_autoScale = enable; }
    
public:
    void onWaveDataChanged();
    void onTimeChanged(double time);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void setupUI();
    void drawGrid(QPainter &painter);
    void drawAxes(QPainter &painter);
    void drawWaveform(QPainter &painter);
    void drawSpectrum(QPainter &painter);
    void drawSuperposition(QPainter &painter);
    void drawLegend(QPainter &painter);
    void drawCursor(QPainter &painter);
    void drawMeasurements(QPainter &painter);
    
    // Coordinate transformation
    QPointF worldToScreen(double x, double y) const;
    QPointF screenToWorld(const QPointF &screen) const;
    void updateTransform();
    
    // Data generation
    void generateTimeData();
    void generateFrequencyData();
    void generateSuperpositionData();
    
    // UI components
    QVBoxLayout *m_layout;
    QFrame *m_plotFrame;
    QHBoxLayout *m_infoLayout;
    QLabel *m_infoLabel;
    
    // Core data
    WaveEngine *m_waveEngine;
    VisualizationMode m_mode;
    
    // Plot data
    std::vector<QPointF> m_plotData;
    std::vector<QPointF> m_spectrumData;
    std::vector<std::vector<QPointF>> m_multiWaveData;
    
    // Display ranges
    double m_minX, m_maxX;
    double m_minY, m_maxY;
    double m_currentTime;
    
    // Display properties
    bool m_showGrid;
    bool m_showLegend;
    bool m_showAxes;
    bool m_autoScale;
    bool m_showCursor;
    
    // Interaction
    bool m_isPanning;
    QPointF m_lastPanPoint;
    QPointF m_cursorPos;
    
    // Plot settings
    QRect m_plotRect;
    QMargins m_plotMargins;
    
    // Colors and styles
    QColor m_backgroundColor;
    QColor m_gridColor;
    QColor m_axesColor;
    QColor m_waveColor;
    QPen m_wavePen;
    QPen m_gridPen;
    QPen m_axesPen;
    
    // Constants
    static constexpr int DEFAULT_POINTS = 1000;
    static constexpr double DEFAULT_DURATION = 5.0;
    static constexpr double ZOOM_FACTOR = 1.2;
};

#endif // WAVE_VISUALIZER_H