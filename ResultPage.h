#ifndef __RESULT_PAGE_H__
#define __RESULT_PAGE_H__

// me
#include "medicalimagefinder_export.h"

//qt
#include <QWizardPage>
#include <QList>

class QTableWidget;

namespace Ui {
	class ResultPage;
}

class MEDICALIMAGEFINDER_EXPORT ResultPage : public QWizardPage
{
	Q_OBJECT

public:
	ResultPage(int numOfImages = 2, QWidget* parent = nullptr);
	ResultPage(QWidget *parent);
	~ResultPage();
	//Ui::ResultPage* getUi() { return this->ui; }

	void initializePage();
	int nextId() const;

private:
	Ui::ResultPage *ui;
	QList<QTableWidget*> m_tableWidgets;
};




#endif // !__RESULT_PAGE_H__
