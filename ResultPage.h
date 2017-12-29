#ifndef __RESULT_PAGE_H__
#define __RESULT_PAGE_H__

//qt
#include <QWizardPage>
#include <QList>

class QTableWidget;

namespace Ui {
	class ResultPage;
}

class ResultPage : public QWizardPage
{
	Q_OBJECT

public:
	ResultPage(int numOfImages = 2, QWidget* parent = nullptr);
	ResultPage(QWidget *parent);
	~ResultPage();
	Ui::ResultPage* getUi() { return this->ui; }

	void initializePage();
	int nextId() const;

private:
	Ui::ResultPage *ui;
	QList<QTableWidget*> m_tableWidgets;
};




#endif // !__RESULT_PAGE_H__
