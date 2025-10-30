#include "MainWindow.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>
#include <QtCore/QStandardPaths>
#include "WaveFunction.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_centralWidget(nullptr)
    , m_waveEngine(std::make_unique<WaveEngine>())
    , m_animationTimer(new QTimer(this))
    , m_isPlaying(false)
    , m_currentTime(0.0)
    , m_animationSpeed(1.0)
    , m_currentWaveIndex(0)
{
    setupUI();
    
    // Initialize with a default sine wave
    m_waveEngine->addWave(std::make_unique<SinusoidalWave>(2.0, 1.0, 0.0));
    
    // Setup animation timer
    m_animationTimer->setInterval(TIMER_INTERVAL);
    connect(m_animationTimer, &QTimer::timeout, [this]() { onAnimationTimer(); });
    
    // Initial update
    updateWaveDisplay();
    updateInfoPanel();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUI() {
    setWindowTitle("🌊 Wave Simulator - Interactive Physics Tool");
    setMinimumSize(1200, 800);
    resize(1400, 900);
    
    // Central widget
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);
    
    m_mainLayout = new QVBoxLayout(m_centralWidget);
    m_topLayout = new QHBoxLayout();
    m_mainLayout->addLayout(m_topLayout);
    
    setupControlPanel();
    setupVisualizationPanel();
    setupStatusPanel();
    
    // Add layouts to main layout
    m_topLayout->addWidget(m_controlGroup, 0);
    m_topLayout->addWidget(m_tabWidget, 1);
    m_topLayout->addWidget(m_infoGroup, 0);
    
    m_mainLayout->addWidget(m_statusFrame);
}

void MainWindow::setupControlPanel() {
    m_controlGroup = new QGroupBox("Wave Controls", this);
    m_controlLayout = new QGridLayout(m_controlGroup);
    
    // Wave type selection
    m_controlLayout->addWidget(new QLabel("Wave Type:"), 0, 0);
    m_waveTypeCombo = new QComboBox();
    m_waveTypeCombo->addItems({"Sinusoidal", "Cosine", "Square", "Triangular", "Sawtooth"});
    m_controlLayout->addWidget(m_waveTypeCombo, 0, 1);
    connect(m_waveTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [this]() { onWaveTypeChanged(); });
    
    // Amplitude control
    m_controlLayout->addWidget(new QLabel("Amplitude:"), 1, 0);
    m_amplitudeSpin = new QDoubleSpinBox();
    m_amplitudeSpin->setRange(MIN_AMPLITUDE, MAX_AMPLITUDE);
    m_amplitudeSpin->setValue(2.0);
    m_amplitudeSpin->setSingleStep(0.1);
    m_amplitudeSpin->setSuffix(" V");
    m_controlLayout->addWidget(m_amplitudeSpin, 1, 1);
    connect(m_amplitudeSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            [this](double value) { onAmplitudeChanged(value); });
    
    m_amplitudeSlider = new QSlider(Qt::Horizontal);
    m_amplitudeSlider->setRange(static_cast<int>(MIN_AMPLITUDE * 10), static_cast<int>(MAX_AMPLITUDE * 10));
    m_amplitudeSlider->setValue(20);
    m_controlLayout->addWidget(m_amplitudeSlider, 1, 2);
    connect(m_amplitudeSlider, &QSlider::valueChanged, [this](int value) {
        m_amplitudeSpin->setValue(value / 10.0);
    });
    
    // Frequency control
    m_controlLayout->addWidget(new QLabel("Frequency:"), 2, 0);
    m_frequencySpin = new QDoubleSpinBox();
    m_frequencySpin->setRange(MIN_FREQUENCY, MAX_FREQUENCY);
    m_frequencySpin->setValue(1.0);
    m_frequencySpin->setSingleStep(0.1);
    m_frequencySpin->setSuffix(" Hz");
    m_controlLayout->addWidget(m_frequencySpin, 2, 1);
    connect(m_frequencySpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            [this](double value) { onFrequencyChanged(value); });
    
    m_frequencySlider = new QSlider(Qt::Horizontal);
    m_frequencySlider->setRange(static_cast<int>(MIN_FREQUENCY * 10), static_cast<int>(MAX_FREQUENCY * 10));
    m_frequencySlider->setValue(10);
    m_controlLayout->addWidget(m_frequencySlider, 2, 2);
    connect(m_frequencySlider, &QSlider::valueChanged, [this](int value) {
        m_frequencySpin->setValue(value / 10.0);
    });
    
    // Phase control
    m_controlLayout->addWidget(new QLabel("Phase:"), 3, 0);
    m_phaseSpin = new QDoubleSpinBox();
    m_phaseSpin->setRange(MIN_PHASE, MAX_PHASE);
    m_phaseSpin->setValue(0.0);
    m_phaseSpin->setSingleStep(15.0);
    m_phaseSpin->setSuffix("°");
    m_controlLayout->addWidget(m_phaseSpin, 3, 1);
    connect(m_phaseSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            [this](double value) { onPhaseChanged(value); });
    
    m_phaseSlider = new QSlider(Qt::Horizontal);
    m_phaseSlider->setRange(static_cast<int>(MIN_PHASE), static_cast<int>(MAX_PHASE));
    m_phaseSlider->setValue(0);
    m_controlLayout->addWidget(m_phaseSlider, 3, 2);
    connect(m_phaseSlider, &QSlider::valueChanged, [this](int value) {
        m_phaseSpin->setValue(static_cast<double>(value));
    });
    
    // Control buttons
    QFrame *buttonFrame = new QFrame();
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonFrame);
    
    m_playPauseButton = new QPushButton("▶ Play");
    m_stopButton = new QPushButton("⏹ Stop");
    m_resetButton = new QPushButton("🔄 Reset");
    m_saveButton = new QPushButton("💾 Save");
    
    buttonLayout->addWidget(m_playPauseButton);
    buttonLayout->addWidget(m_stopButton);
    buttonLayout->addWidget(m_resetButton);
    buttonLayout->addWidget(m_saveButton);
    
    connect(m_playPauseButton, &QPushButton::clicked, [this]() { onPlayPauseClicked(); });
    connect(m_stopButton, &QPushButton::clicked, [this]() { onStopClicked(); });
    connect(m_resetButton, &QPushButton::clicked, [this]() { onResetClicked(); });
    connect(m_saveButton, &QPushButton::clicked, [this]() { onSaveClicked(); });
    
    m_controlLayout->addWidget(buttonFrame, 4, 0, 1, 3);
    
    // Wave management buttons
    QFrame *waveFrame = new QFrame();
    QHBoxLayout *waveLayout = new QHBoxLayout(waveFrame);
    
    m_addWaveButton = new QPushButton("+ Add Wave");
    m_removeWaveButton = new QPushButton("- Remove");
    m_clearWavesButton = new QPushButton("Clear All");
    
    waveLayout->addWidget(m_addWaveButton);
    waveLayout->addWidget(m_removeWaveButton);
    waveLayout->addWidget(m_clearWavesButton);
    
    connect(m_addWaveButton, &QPushButton::clicked, [this]() { onAddWaveClicked(); });
    connect(m_removeWaveButton, &QPushButton::clicked, [this]() { onRemoveWaveClicked(); });
    connect(m_clearWavesButton, &QPushButton::clicked, [this]() { onClearWavesClicked(); });
    
    m_controlLayout->addWidget(waveFrame, 5, 0, 1, 3);
}

void MainWindow::setupVisualizationPanel() {
    m_tabWidget = new QTabWidget(this);
    
    // Simple wave tab
    m_simpleWaveTab = new WaveVisualizer();
    m_simpleWaveTab->setWaveEngine(m_waveEngine.get());
    m_simpleWaveTab->setVisualizationMode(VisualizationMode::TIME_DOMAIN);
    m_tabWidget->addTab(m_simpleWaveTab, "🌊 Simple Wave");
    
    // Superposition tab
    m_superpositionTab = new WaveVisualizer();
    m_superpositionTab->setWaveEngine(m_waveEngine.get());
    m_superpositionTab->setVisualizationMode(VisualizationMode::SUPERPOSITION);
    m_tabWidget->addTab(m_superpositionTab, "🔄 Superposition");
    
    // Spectrum tab
    m_spectrumTab = new WaveVisualizer();
    m_spectrumTab->setWaveEngine(m_waveEngine.get());
    m_spectrumTab->setVisualizationMode(VisualizationMode::FREQUENCY_DOMAIN);
    m_tabWidget->addTab(m_spectrumTab, "📊 Spectrum");
    
    // Theory tab
    m_theoryTab = new QTextEdit();
    m_theoryTab->setReadOnly(true);
    m_theoryTab->setHtml(R"(
        <h2>🌊 Wave Theory</h2>
        <h3>Basic Wave Equation</h3>
        <p><b>y(x,t) = A sin(kx - ωt + φ)</b></p>
        <ul>
        <li><b>A</b>: Amplitude (maximum displacement)</li>
        <li><b>k = 2π/λ</b>: Wave number</li>
        <li><b>ω = 2πf</b>: Angular frequency</li>
        <li><b>φ</b>: Phase constant</li>
        </ul>
        
        <h3>Important Relations</h3>
        <ul>
        <li><b>v = fλ</b>: Wave speed = frequency × wavelength</li>
        <li><b>T = 1/f</b>: Period = inverse frequency</li>
        <li><b>E ∝ A²</b>: Energy proportional to amplitude squared</li>
        </ul>
        
        <h3>🔄 Interference</h3>
        <ul>
        <li><b>Constructive</b>: Δφ = 2nπ (waves in phase)</li>
        <li><b>Destructive</b>: Δφ = (2n+1)π (waves out of phase)</li>
        <li><b>Beating</b>: f_beat = |f₁ - f₂| (close frequencies)</li>
        </ul>
        
        <h3>📊 Fourier Analysis</h3>
        <p>Any periodic wave can be decomposed into sine and cosine components:</p>
        <p><b>f(t) = a₀ + Σ(aₙcos(nωt) + bₙsin(nωt))</b></p>
    )");
    m_tabWidget->addTab(m_theoryTab, "📚 Theory");
    
    connect(m_tabWidget, &QTabWidget::currentChanged, [this](int index) { onTabChanged(index); });
}

void MainWindow::setupStatusPanel() {
    // Information panel
    m_infoGroup = new QGroupBox("Wave Information", this);
    m_infoLayout = new QGridLayout(m_infoGroup);
    
    m_timeLabel = new QLabel("Time: 0.00 s");
    m_amplitudeLabel = new QLabel("Amplitude: 2.00 V");
    m_frequencyLabel = new QLabel("Frequency: 1.00 Hz");
    m_periodLabel = new QLabel("Period: 1.00 s");
    m_wavelengthLabel = new QLabel("Wavelength: 1.00 m");
    m_energyLabel = new QLabel("Energy: 2.00 J");
    m_phenomenonLabel = new QLabel("Phenomenon: Single wave");
    
    m_infoLayout->addWidget(m_timeLabel, 0, 0);
    m_infoLayout->addWidget(m_amplitudeLabel, 1, 0);
    m_infoLayout->addWidget(m_frequencyLabel, 2, 0);
    m_infoLayout->addWidget(m_periodLabel, 3, 0);
    m_infoLayout->addWidget(m_wavelengthLabel, 4, 0);
    m_infoLayout->addWidget(m_energyLabel, 5, 0);
    m_infoLayout->addWidget(m_phenomenonLabel, 6, 0);
    
    // Status bar
    m_statusFrame = new QFrame();
    m_statusFrame->setFrameStyle(QFrame::Box);
    m_statusLayout = new QHBoxLayout(m_statusFrame);
    
    m_statusLabel = new QLabel("Ready");
    m_progressBar = new QProgressBar();
    m_progressBar->setVisible(false);
    
    m_statusLayout->addWidget(m_statusLabel);
    m_statusLayout->addStretch();
    m_statusLayout->addWidget(m_progressBar);
}

void MainWindow::onWaveTypeChanged() {
    updateWaveParameters();
}

void MainWindow::onAmplitudeChanged(double value) {
    m_amplitudeSlider->setValue(static_cast<int>(value * 10));
    updateWaveParameters();
}

void MainWindow::onFrequencyChanged(double value) {
    m_frequencySlider->setValue(static_cast<int>(value * 10));
    updateWaveParameters();
}

void MainWindow::onPhaseChanged(double value) {
    m_phaseSlider->setValue(static_cast<int>(value));
    updateWaveParameters();
}

void MainWindow::onPlayPauseClicked() {
    if (m_isPlaying) {
        m_animationTimer->stop();
        m_playPauseButton->setText("▶ Play");
        m_isPlaying = false;
        m_statusLabel->setText("Paused");
    } else {
        m_animationTimer->start();
        m_playPauseButton->setText("⏸ Pause");
        m_isPlaying = true;
        m_statusLabel->setText("Playing...");
    }
}

void MainWindow::onStopClicked() {
    m_animationTimer->stop();
    m_playPauseButton->setText("▶ Play");
    m_isPlaying = false;
    m_currentTime = 0.0;
    m_statusLabel->setText("Stopped");
    updateWaveDisplay();
    updateInfoPanel();
}

void MainWindow::onResetClicked() {
    onStopClicked();
    m_waveEngine->clearWaves();
    m_waveEngine->addWave(std::make_unique<SinusoidalWave>(2.0, 1.0, 0.0));
    m_waveTypeCombo->setCurrentIndex(0);
    m_amplitudeSpin->setValue(2.0);
    m_frequencySpin->setValue(1.0);
    m_phaseSpin->setValue(0.0);
    updateWaveDisplay();
    updateInfoPanel();
    m_statusLabel->setText("Reset to defaults");
}

void MainWindow::onSaveClicked() {
    QString fileName = QFileDialog::getSaveFileName(this, "Save Wave Data", 
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/wave_data.csv",
        "CSV Files (*.csv);;All Files (*)");
    
    if (!fileName.isEmpty()) {
        // TODO: Implement save functionality
        m_statusLabel->setText("Saved to: " + fileName);
    }
}

void MainWindow::onAddWaveClicked() {
    WaveType type = static_cast<WaveType>(m_waveTypeCombo->currentIndex());
    double amplitude = m_amplitudeSpin->value();
    double frequency = m_frequencySpin->value();
    double phase = m_phaseSpin->value();
    
    std::unique_ptr<WaveFunction> wave;
    switch (type) {
        case WaveType::SINUSOIDAL:
            wave = std::make_unique<SinusoidalWave>(amplitude, frequency, phase);
            break;
        case WaveType::COSINE:
            wave = std::make_unique<CosineWave>(amplitude, frequency, phase);
            break;
        case WaveType::SQUARE:
            wave = std::make_unique<SquareWave>(amplitude, frequency, phase);
            break;
        case WaveType::TRIANGULAR:
            wave = std::make_unique<TriangularWave>(amplitude, frequency, phase);
            break;
        case WaveType::SAWTOOTH:
            wave = std::make_unique<SawtoothWave>(amplitude, frequency, phase);
            break;
        default:
            wave = std::make_unique<SinusoidalWave>(amplitude, frequency, phase);
    }
    
    m_waveEngine->addWave(std::move(wave));
    updateWaveDisplay();
    updateInfoPanel();
    m_statusLabel->setText(QString("Added wave. Total: %1").arg(m_waveEngine->getWaveCount()));
}

void MainWindow::onRemoveWaveClicked() {
    if (m_waveEngine->getWaveCount() > 1) {
        m_waveEngine->removeWave(m_waveEngine->getWaveCount() - 1);
        updateWaveDisplay();
        updateInfoPanel();
        m_statusLabel->setText(QString("Removed wave. Total: %1").arg(m_waveEngine->getWaveCount()));
    }
}

void MainWindow::onClearWavesClicked() {
    m_waveEngine->clearWaves();
    m_waveEngine->addWave(std::make_unique<SinusoidalWave>(2.0, 1.0, 0.0));
    updateWaveDisplay();
    updateInfoPanel();
    m_statusLabel->setText("Cleared all waves, added default sine wave");
}

void MainWindow::onTabChanged(int index) {
    Q_UNUSED(index)
    updateWaveDisplay();
}

void MainWindow::onAnimationTimer() {
    m_currentTime += TIMER_INTERVAL / 1000.0 * m_animationSpeed;
    updateWaveDisplay();
    updateInfoPanel();
}

void MainWindow::updateWaveParameters() {
    if (m_waveEngine->getWaveCount() > 0) {
        WaveFunction* wave = const_cast<WaveFunction*>(m_waveEngine->getWave(0));
        if (wave) {
            wave->setAmplitude(m_amplitudeSpin->value());
            wave->setFrequency(m_frequencySpin->value());
            wave->setPhase(m_phaseSpin->value());
            updateWaveDisplay();
            updateInfoPanel();
        }
    }
}

void MainWindow::updateWaveDisplay() {
    m_simpleWaveTab->setCurrentTime(m_currentTime);
    m_superpositionTab->setCurrentTime(m_currentTime);
    m_spectrumTab->setCurrentTime(m_currentTime);
    
    m_simpleWaveTab->updateVisualization();
    m_superpositionTab->updateVisualization();
    m_spectrumTab->updateVisualization();
}

void MainWindow::updateInfoPanel() {
    m_timeLabel->setText(QString("Time: %1 s").arg(m_currentTime, 0, 'f', 2));
    
    if (m_waveEngine->getWaveCount() > 0) {
        const WaveFunction* wave = m_waveEngine->getWave(0);
        if (wave) {
            m_amplitudeLabel->setText(QString("Amplitude: %1 V").arg(wave->getAmplitude(), 0, 'f', 2));
            m_frequencyLabel->setText(QString("Frequency: %1 Hz").arg(wave->getFrequency(), 0, 'f', 2));
            m_periodLabel->setText(QString("Period: %1 s").arg(wave->getPeriod(), 0, 'f', 2));
            m_wavelengthLabel->setText(QString("Wavelength: %1 m").arg(wave->getWavelength(), 0, 'f', 2));
            m_energyLabel->setText(QString("Energy: %1 J").arg(wave->getEnergy(), 0, 'f', 2));
        }
    }
    
    m_phenomenonLabel->setText(QString("Phenomenon: %1").arg(QString::fromStdString(m_waveEngine->detectPhenomenon())));
}

void MainWindow::updateStatus() {
    // Update status as needed
}

