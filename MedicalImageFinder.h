#ifndef __MEDICAL_IMAGE_FINDER_H__
#define __MEDICAL_IMAGE_FINDER_H__

// me
//#include "registrationwizard_export.h"

// qt
#include <QWizard>
#include <QRunnable>
#include <QLabel>
#include <QLineEdit>

class DirectoryPage;
class ImagePage;
class ResultPage;


namespace Ui {
	class MedicalImageFinder;
}

class MedicalImageFinder : public QWizard
{
	Q_OBJECT

public:
	enum {
		/// the first page
		PAGE_DIRECTORY = 0,
		/// the second page
		PAGE_IMAGE = 1,
		/// the last
		PAGE_RESULT = 2
	};
	static const QString SEPERATOR;

	MedicalImageFinder(QString dir = QString(), int numOfImages = 2, QWidget *parent = nullptr);
	MedicalImageFinder(int numOfImages, QWidget *parent = nullptr);
	MedicalImageFinder(QWidget *parent);
	~MedicalImageFinder();
	Ui::MedicalImageFinder* getUi() { return this->ui; }

	void setImageModalityNames(unsigned int i, QString imageModalityName = QString());

	QString getFileNames(unsigned int i);
	void setDirectory(QString directory);
	const QString getDirectory();
	int getNumberOfImages();

private slots:
	void showHelp();

private:
	Ui::MedicalImageFinder *ui;
	int m_numOfImages;
	DirectoryPage	*directoryPage;
	ImagePage		*imagePage;
	ResultPage		*resultPage;
};




#endif // !__MEDICAL_IMAGE_FINDER_H__
