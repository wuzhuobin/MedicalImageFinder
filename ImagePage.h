#ifndef __IMAGE_PAGE_H__
#define __IMAGE_PAGE_H__

// me
#include "medicalimagefinder_export.h"

//qt
#include <QWizardPage>
#include <QRunnable>
#include <QList>

class QLabel;
class QLineEdit;
class QThreadPool;
//class FindImageRunnable;

namespace Ui {
	class ImagePage;
}

class MEDICALIMAGEFINDER_EXPORT ImagePage : public QWizardPage
{
	Q_OBJECT

public:
	ImagePage(int numOfImages = 2, QWidget* parent = nullptr);
	ImagePage(QWidget *parent);
	~ImagePage();

	void setImageModalityNames(unsigned int i, QString imageModalityName);

	int nextId() const;

	virtual void initializePage();
	virtual bool validatePage();

	Ui::ImagePage* getUi() { return this->ui; }

public slots:
	void updateProgressBar();
	void setImages();
	void removeImages();
	void updateTreeWidget(QString item);

signals:
	void finishPrevious();

private:
	Ui::ImagePage *ui;

	QList<QLabel*> m_imageLabels;
	QList<QLineEdit*> m_imageLineEdits;
	QList<QLineEdit*> m_imageLineEdits2;
	QList<QPushButton*> m_imageSetBtns;
	QList<QPushButton*> m_imageRemoveBtns;

	QThreadPool* m_threadPool;
	QString m_lastDirectory;
};


class FindImageRunnable :public QObject, public QRunnable
{
	Q_OBJECT;
public:
	FindImageRunnable(QString folderPath, QObject* parent = nullptr);
	void run();

	public slots:
	void forceFinish();

signals:
	void addTreeWidgetItem(QString);
	void finish();
private:
	void GDCImageIORead(const std::vector<std::string>& files);
	QString m_folderPath;
	bool m_forceFinish = false;
};


#endif // !__IMAGE_PAGE_H__
