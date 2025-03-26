#ifndef MUON_HISTORYDIALOG_H
#define MUON_HISTORYDIALOG_H

#include <QDialog>

class HistoryDialog : public QDialog {
Q_OBJECT

public:
    explicit HistoryDialog(QWidget *parent);

protected:
    ~HistoryDialog() override;
};

#endif
