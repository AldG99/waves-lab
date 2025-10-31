#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSlider>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QFrame>
#include <QtCore/QTimer>
#include <memory>
#include "WaveEngine.h"
#include "WaveVisualizer.h"

class MainWindow : public QMainWindow {

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void onWaveTypeChanged();
    void onAmplitudeChanged(double value);
    void onFrequencyChanged(double value);
    void onPhaseChanged(double value);
    void onPlayPauseClicked();
    void onStopClicked();
    void onResetClicked();
    void onSaveClicked();
    void onAddWaveClicked();
    void onRemoveWaveClicked();
    void onClearWavesClicked();
    void onTabChanged(int index);
    void onAnimationTimer();

private:
    void setupUI();
    void setupControlPanel();
    void setupVisualizationPanel();
    void setupStatusPanel();
    void updateWaveParameters();
    void updateWaveDisplay();
    void updateInfoPanel();
    void updateStatus();
    
    // Central widget and layouts
    QWidget *m_centralWidget;
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_topLayout;
    
    // Control Panel
    QGroupBox *m_controlGroup;
    QGridLayout *m_controlLayout;
    QComboBox *m_waveTypeCombo;
    QDoubleSpinBox *m_amplitudeSpin;
    QDoubleSpinBox *m_frequencySpin;
    QDoubleSpinBox *m_phaseSpin;
    QSlider *m_amplitudeSlider;
    QSlider *m_frequencySlider;
    QSlider *m_phaseSlider;
    QPushButton *m_playPauseButton;
    QPushButton *m_stopButton;
    QPushButton *m_resetButton;
    QPushButton *m_saveButton;
    QPushButton *m_addWaveButton;
    QPushButton *m_removeWaveButton;
    QPushButton *m_clearWavesButton;
    
    // Visualization Panel
    QTabWidget *m_tabWidget;
    WaveVisualizer *m_simpleWaveTab;
    WaveVisualizer *m_superpositionTab;
    WaveVisualizer *m_spectrumTab;
    QTextEdit *m_theoryTab;
    
    // Information Panel
    QGroupBox *m_infoGroup;
    QGridLayout *m_infoLayout;
    QLabel *m_timeLabel;
    QLabel *m_amplitudeLabel;
    QLabel *m_frequencyLabel;
    QLabel *m_periodLabel;
    QLabel *m_wavelengthLabel;
    QLabel *m_energyLabel;
    QLabel *m_phenomenonLabel;
    
    // Status Panel
    QFrame *m_statusFrame;
    QHBoxLayout *m_statusLayout;
    QLabel *m_statusLabel;
    QProgressBar *m_progressBar;
    
    // Core components
    std::unique_ptr<WaveEngine> m_waveEngine;
    QTimer *m_animationTimer;
    
    // State variables
    bool m_isPlaying;
    double m_currentTime;
    double m_animationSpeed;
    int m_currentWaveIndex;
    
    // Constants
    static constexpr double MIN_AMPLITUDE = 0.1;
    static constexpr double MAX_AMPLITUDE = 10.0;
    static constexpr double MIN_FREQUENCY = 0.1;
    static constexpr double MAX_FREQUENCY = 10.0;
    static constexpr double MIN_PHASE = 0.0;
    static constexpr double MAX_PHASE = 360.0;
    static constexpr int TIMER_INTERVAL = 50; // ms (20 FPS)
};

#endif // MAINWINDOW_H