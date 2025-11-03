#include "WaveVisualizer.h"
#include <QtGui/QPainter>
#include <QtGui/QPen>
#include <QtGui/QBrush>
#include <QtCore/QDebug>
#include <cmath>

WaveVisualizer::WaveVisualizer(QWidget *parent)
    : QWidget(parent)
    , m_layout(nullptr)
    , m_plotFrame(nullptr)
    , m_infoLayout(nullptr)
    , m_infoLabel(nullptr)
    , m_waveEngine(nullptr)
    , m_mode(VisualizationMode::TIME_DOMAIN)
    , m_minX(-5.0)
    , m_maxX(5.0)
    , m_minY(-5.0)
    , m_maxY(5.0)
    , m_currentTime(0.0)
    , m_showGrid(true)
    , m_showLegend(true)
    , m_showAxes(true)
    , m_autoScale(true)
    , m_showCursor(false)
    , m_isPanning(false)
    , m_plotMargins(60, 40, 40, 60)
    , m_backgroundColor(Qt::white)
    , m_gridColor(240, 240, 240)
    , m_axesColor(Qt::black)
    , m_waveColor(Qt::blue)
{
    // Configure pens
    m_wavePen = QPen(m_waveColor, 2);
    m_gridPen = QPen(m_gridColor, 1, Qt::DotLine);
    m_axesPen = QPen(m_axesColor, 2);
    
    setMinimumSize(400, 300);
    
    setupUI();
}

WaveVisualizer::~WaveVisualizer() = default;

void WaveVisualizer::setupUI() {
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    
    // Main plot area
    m_plotFrame = new QFrame();
    m_plotFrame->setFrameStyle(QFrame::Box);
    m_plotFrame->setLineWidth(1);
    m_layout->addWidget(m_plotFrame, 1);
    
    // Info area
    m_infoLayout = new QHBoxLayout();
    m_infoLabel = new QLabel("Ready");
    m_infoLabel->setStyleSheet("QLabel { padding: 5px; background-color: #f0f0f0; }");
    m_infoLayout->addWidget(m_infoLabel);
    m_layout->addLayout(m_infoLayout);
}

void WaveVisualizer::setWaveEngine(WaveEngine* engine) {
    m_waveEngine = engine;
    updateVisualization();
}

void WaveVisualizer::setVisualizationMode(VisualizationMode mode) {
    m_mode = mode;
    updateVisualization();
}

void WaveVisualizer::setTimeRange(double startTime, double endTime) {
    if (m_mode == VisualizationMode::TIME_DOMAIN) {
        m_minX = startTime;
        m_maxX = endTime;
        updateTransform();
        update();
    }
}

void WaveVisualizer::setFrequencyRange(double startFreq, double endFreq) {
    if (m_mode == VisualizationMode::FREQUENCY_DOMAIN) {
        m_minX = startFreq;
        m_maxX = endFreq;
        updateTransform();
        update();
    }
}

void WaveVisualizer::setCurrentTime(double time) {
    m_currentTime = time;
    updateVisualization();
}

void WaveVisualizer::updateVisualization() {
    if (!m_waveEngine) return;
    
    switch (m_mode) {
        case VisualizationMode::TIME_DOMAIN:
            generateTimeData();
            break;
        case VisualizationMode::FREQUENCY_DOMAIN:
            generateFrequencyData();
            break;
        case VisualizationMode::SUPERPOSITION:
            generateSuperpositionData();
            break;
        case VisualizationMode::INTERFERENCE_PATTERN:
            generateTimeData(); // For now, same as time domain
            break;
    }
    
    updateTransform();
    update();
}

void WaveVisualizer::onWaveDataChanged() {
    updateVisualization();
}

void WaveVisualizer::onTimeChanged(double time) {
    setCurrentTime(time);
}

void WaveVisualizer::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Fill background
    painter.fillRect(rect(), m_backgroundColor);
    
    // Update plot rectangle
    m_plotRect = rect().marginsRemoved(m_plotMargins);
    
    // Draw components
    if (m_showGrid) drawGrid(painter);
    if (m_showAxes) drawAxes(painter);
    
    switch (m_mode) {
        case VisualizationMode::TIME_DOMAIN:
            drawWaveform(painter);
            break;
        case VisualizationMode::FREQUENCY_DOMAIN:
            drawSpectrum(painter);
            break;
        case VisualizationMode::SUPERPOSITION:
            drawSuperposition(painter);
            break;
        case VisualizationMode::INTERFERENCE_PATTERN:
            drawWaveform(painter);
            break;
    }
    
    if (m_showCursor) drawCursor(painter);
    if (m_showLegend) drawLegend(painter);
    drawMeasurements(painter);
}

void WaveVisualizer::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_isPanning = true;
        m_lastPanPoint = event->pos();
        m_cursorPos = screenToWorld(event->pos());
        m_showCursor = true;
        update();
    }
}

void WaveVisualizer::mouseMoveEvent(QMouseEvent *event) {
    if (m_isPanning && (event->buttons() & Qt::LeftButton)) {
        QPointF delta = event->pos() - m_lastPanPoint;
        
        // Convert screen delta to world delta
        double worldDeltaX = delta.x() * (m_maxX - m_minX) / m_plotRect.width();
        double worldDeltaY = -delta.y() * (m_maxY - m_minY) / m_plotRect.height();
        
        m_minX -= worldDeltaX;
        m_maxX -= worldDeltaX;
        m_minY -= worldDeltaY;
        m_maxY -= worldDeltaY;
        
        m_lastPanPoint = event->pos();
        updateTransform();
        update();
    } else {
        m_cursorPos = screenToWorld(event->pos());
        if (m_showCursor) update();
    }
}

void WaveVisualizer::wheelEvent(QWheelEvent *event) {
    if (m_plotRect.contains(event->pos())) {
        double factor = (event->delta() > 0) ? (1.0 / ZOOM_FACTOR) : ZOOM_FACTOR;
        
        QPointF center = screenToWorld(event->pos());
        double rangeX = m_maxX - m_minX;
        double rangeY = m_maxY - m_minY;
        
        double newRangeX = rangeX * factor;
        double newRangeY = rangeY * factor;
        
        m_minX = center.x() - newRangeX * (center.x() - m_minX) / rangeX;
        m_maxX = center.x() + newRangeX * (m_maxX - center.x()) / rangeX;
        m_minY = center.y() - newRangeY * (center.y() - m_minY) / rangeY;
        m_maxY = center.y() + newRangeY * (m_maxY - center.y()) / rangeY;
        
        updateTransform();
        update();
    }
}

void WaveVisualizer::resizeEvent(QResizeEvent *event) {
    Q_UNUSED(event)
    updateTransform();
}

void WaveVisualizer::drawGrid(QPainter &painter) {
    painter.setPen(m_gridPen);
    
    // Calculate grid spacing
    double rangeX = m_maxX - m_minX;
    double rangeY = m_maxY - m_minY;
    
    double gridX = pow(10, floor(log10(rangeX)) - 1);
    double gridY = pow(10, floor(log10(rangeY)) - 1);
    
    // Vertical lines
    for (double x = ceil(m_minX / gridX) * gridX; x <= m_maxX; x += gridX) {
        QPointF screenPt = worldToScreen(x, 0);
        painter.drawLine(static_cast<int>(screenPt.x()), m_plotRect.top(),
                        static_cast<int>(screenPt.x()), m_plotRect.bottom());
    }
    
    // Horizontal lines
    for (double y = ceil(m_minY / gridY) * gridY; y <= m_maxY; y += gridY) {
        QPointF screenPt = worldToScreen(0, y);
        painter.drawLine(m_plotRect.left(), static_cast<int>(screenPt.y()),
                        m_plotRect.right(), static_cast<int>(screenPt.y()));
    }
}

void WaveVisualizer::drawAxes(QPainter &painter) {
    painter.setPen(m_axesPen);
    
    // X-axis
    if (m_minY <= 0 && m_maxY >= 0) {
        QPointF screenPt = worldToScreen(0, 0);
        painter.drawLine(m_plotRect.left(), static_cast<int>(screenPt.y()),
                        m_plotRect.right(), static_cast<int>(screenPt.y()));
    }
    
    // Y-axis
    if (m_minX <= 0 && m_maxX >= 0) {
        QPointF screenPt = worldToScreen(0, 0);
        painter.drawLine(static_cast<int>(screenPt.x()), m_plotRect.top(),
                        static_cast<int>(screenPt.x()), m_plotRect.bottom());
    }
}

void WaveVisualizer::drawWaveform(QPainter &painter) {
    if (m_plotData.empty()) return;
    
    painter.setPen(m_wavePen);
    
    for (size_t i = 1; i < m_plotData.size(); ++i) {
        QPointF pt1 = worldToScreen(m_plotData[i-1].x(), m_plotData[i-1].y());
        QPointF pt2 = worldToScreen(m_plotData[i].x(), m_plotData[i].y());
        
        if (m_plotRect.contains(pt1.toPoint()) || m_plotRect.contains(pt2.toPoint())) {
            painter.drawLine(pt1, pt2);
        }
    }
}

void WaveVisualizer::drawSpectrum(QPainter &painter) {
    if (m_spectrumData.empty()) return;
    
    painter.setPen(m_wavePen);
    painter.setBrush(QBrush(m_waveColor.lighter(150)));
    
    for (const auto& point : m_spectrumData) {
        QPointF screenPt = worldToScreen(point.x(), point.y());
        QPointF basePt = worldToScreen(point.x(), 0);
        
        if (m_plotRect.contains(screenPt.toPoint())) {
            painter.drawLine(basePt, screenPt);
        }
    }
}

void WaveVisualizer::drawSuperposition(QPainter &painter) {
    // Draw individual waves with different colors
    QColor colors[] = {Qt::blue, Qt::red, Qt::green, Qt::magenta, Qt::cyan};
    
    for (size_t waveIdx = 0; waveIdx < m_multiWaveData.size(); ++waveIdx) {
        QPen pen(colors[waveIdx % 5], 1, Qt::DashLine);
        painter.setPen(pen);
        
        const auto& waveData = m_multiWaveData[waveIdx];
        for (size_t i = 1; i < waveData.size(); ++i) {
            QPointF pt1 = worldToScreen(waveData[i-1].x(), waveData[i-1].y());
            QPointF pt2 = worldToScreen(waveData[i].x(), waveData[i].y());
            
            if (m_plotRect.contains(pt1.toPoint()) || m_plotRect.contains(pt2.toPoint())) {
                painter.drawLine(pt1, pt2);
            }
        }
    }
    
    // Draw superposition with thick line
    painter.setPen(QPen(Qt::black, 3));
    drawWaveform(painter);
}

void WaveVisualizer::drawLegend(QPainter &painter) {
    if (!m_waveEngine) return;
    
    QRect legendRect(m_plotRect.right() - 200, m_plotRect.top() + 10, 190, 20 * m_waveEngine->getWaveCount() + 20);
    
    painter.setPen(Qt::black);
    painter.setBrush(QBrush(Qt::white, Qt::SolidPattern));
    painter.drawRect(legendRect);
    
    painter.drawText(legendRect.left() + 5, legendRect.top() + 15, "Waves:");
    
    for (size_t i = 0; i < m_waveEngine->getWaveCount(); ++i) {
        const WaveFunction* wave = m_waveEngine->getWave(i);
        if (wave) {
            QColor color = (i < 5) ? QColor(Qt::GlobalColor(Qt::blue + i)) : Qt::black;
            painter.setPen(color);
            
            QString text = QString("Wave %1: A=%2, f=%3 Hz")
                          .arg(i + 1)
                          .arg(wave->getAmplitude(), 0, 'f', 1)
                          .arg(wave->getFrequency(), 0, 'f', 1);
            
            painter.drawText(legendRect.left() + 5, legendRect.top() + 30 + i * 20, text);
        }
    }
}

void WaveVisualizer::drawCursor(QPainter &painter) {
    if (!m_showCursor) return;
    
    painter.setPen(QPen(Qt::red, 1, Qt::DashLine));
    QPointF screenPt = worldToScreen(m_cursorPos.x(), m_cursorPos.y());
    
    // Cross-hair
    painter.drawLine(m_plotRect.left(), static_cast<int>(screenPt.y()),
                    m_plotRect.right(), static_cast<int>(screenPt.y()));
    painter.drawLine(static_cast<int>(screenPt.x()), m_plotRect.top(),
                    static_cast<int>(screenPt.x()), m_plotRect.bottom());
}

void WaveVisualizer::drawMeasurements(QPainter &painter) {
    if (!m_waveEngine) return;
    
    QString info;
    switch (m_mode) {
        case VisualizationMode::TIME_DOMAIN:
            info = QString("Time: %1 s | Waves: %2")
                   .arg(m_currentTime, 0, 'f', 2)
                   .arg(m_waveEngine->getWaveCount());
            break;
        case VisualizationMode::FREQUENCY_DOMAIN:
            info = QString("Frequency Domain | Dominant: %1 Hz")
                   .arg(m_waveEngine->getDominantFrequency(), 0, 'f', 2);
            break;
        case VisualizationMode::SUPERPOSITION:
            info = QString("Superposition | Phenomenon: %1")
                   .arg(QString::fromStdString(m_waveEngine->detectPhenomenon()));
            break;
        default:
            info = "Wave Visualizer";
    }
    
    m_infoLabel->setText(info);
}

QPointF WaveVisualizer::worldToScreen(double x, double y) const {
    if (m_plotRect.isEmpty()) return QPointF(0, 0);
    
    double screenX = m_plotRect.left() + (x - m_minX) * m_plotRect.width() / (m_maxX - m_minX);
    double screenY = m_plotRect.bottom() - (y - m_minY) * m_plotRect.height() / (m_maxY - m_minY);
    
    return QPointF(screenX, screenY);
}

QPointF WaveVisualizer::screenToWorld(const QPointF &screen) const {
    if (m_plotRect.isEmpty()) return QPointF(0, 0);
    
    double worldX = m_minX + (screen.x() - m_plotRect.left()) * (m_maxX - m_minX) / m_plotRect.width();
    double worldY = m_maxY - (screen.y() - m_plotRect.top()) * (m_maxY - m_minY) / m_plotRect.height();
    
    return QPointF(worldX, worldY);
}

void WaveVisualizer::updateTransform() {
    if (m_autoScale && m_waveEngine && !m_plotData.empty()) {
        // Auto-scale Y axis based on data
        double minY = std::numeric_limits<double>::max();
        double maxY = std::numeric_limits<double>::lowest();
        
        for (const auto& point : m_plotData) {
            minY = std::min(minY, point.y());
            maxY = std::max(maxY, point.y());
        }
        
        double margin = (maxY - minY) * 0.1;
        m_minY = minY - margin;
        m_maxY = maxY + margin;
    }
    
}

void WaveVisualizer::generateTimeData() {
    if (!m_waveEngine) return;
    
    m_plotData.clear();
    
    double duration = m_maxX - m_minX;
    int numPoints = static_cast<int>(std::min(duration * 100, static_cast<double>(DEFAULT_POINTS)));
    double dt = duration / numPoints;
    
    for (int i = 0; i <= numPoints; ++i) {
        double t = m_minX + i * dt;
        double amplitude = m_waveEngine->evaluateSuperposition(0.0, t);
        m_plotData.emplace_back(t, amplitude);
    }
}

void WaveVisualizer::generateFrequencyData() {
    if (!m_waveEngine) return;
    
    // Generate time series for FFT
    auto timeSeries = m_waveEngine->generateTimeSeries(DEFAULT_DURATION, 256.0);
    
    // Perform FFT (simplified - using dominant frequency approach)
    m_spectrumData.clear();
    
    for (size_t i = 0; i < m_waveEngine->getWaveCount(); ++i) {
        const WaveFunction* wave = m_waveEngine->getWave(i);
        if (wave) {
            double freq = wave->getFrequency();
            double amp = wave->getAmplitude();
            m_spectrumData.emplace_back(freq, amp);
        }
    }
    
    // Set appropriate frequency range
    if (m_autoScale && !m_spectrumData.empty()) {
        m_minX = 0;
        m_maxX = m_waveEngine->getDominantFrequency() * 5;
        m_minY = 0;
        m_maxY = m_waveEngine->getMaxAmplitude() * 1.2;
    }
}

void WaveVisualizer::generateSuperpositionData() {
    if (!m_waveEngine) return;
    
    // Generate individual wave data
    m_multiWaveData.clear();
    
    double duration = m_maxX - m_minX;
    int numPoints = static_cast<int>(std::min(duration * 100, static_cast<double>(DEFAULT_POINTS)));
    double dt = duration / numPoints;
    
    for (size_t waveIdx = 0; waveIdx < m_waveEngine->getWaveCount(); ++waveIdx) {
        std::vector<QPointF> waveData;
        
        for (int i = 0; i <= numPoints; ++i) {
            double t = m_minX + i * dt;
            double amplitude = m_waveEngine->evaluateWave(waveIdx, 0.0, t);
            waveData.emplace_back(t, amplitude);
        }
        
        m_multiWaveData.push_back(waveData);
    }
    
    // Generate superposition
    generateTimeData();
}

