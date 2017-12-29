#include "ui_MedicalImageFinder.h"
#include "MedicalImageFinder.h"

//qt
#include <QDebug>
#include <QDirIterator>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QThreadPool>

//me
#include "DirectoryPage.h"
#include "ImagePage.h"
#include "ResultPage.h"

const QString MedicalImageFinder::SEPERATOR(";");

MedicalImageFinder::MedicalImageFinder(QString dir, int numOfImages, QWidget *parent) :
	m_numOfImages(numOfImages),
	QWizard(parent),
	ui(new Ui::MedicalImageFinder)
{
	ui->setupUi(this);
	directoryPage = new DirectoryPage(dir, this);
	imagePage = new ImagePage(numOfImages, this);
	resultPage = new ResultPage(numOfImages, this);
	
	setPage(PAGE_DIRECTORY, directoryPage);
	setPage(PAGE_IMAGE, imagePage);
	setPage(PAGE_RESULT, resultPage);

	setStartId(PAGE_DIRECTORY);
	connect(this, SIGNAL(helpRequested()), this, SLOT(showHelp()));
}

MedicalImageFinder::MedicalImageFinder(int numOfImages, QWidget * parent)
	:MedicalImageFinder(QString(), numOfImages, parent)
{
}

MedicalImageFinder::MedicalImageFinder(QWidget * parent)
	: MedicalImageFinder(QString(), 2, parent)
{
}

MedicalImageFinder::~MedicalImageFinder()
{
	delete ui;
}

void MedicalImageFinder::setImageModalityNames(unsigned int i, QString imageModalityName)
{
	imagePage->setImageModalityNames(i, imageModalityName);
}

void MedicalImageFinder::showHelp()
{
	QString message;

	switch (currentId()) {
	case PAGE_DIRECTORY:
		message = tr("Select the image directory");
		break;
	case PAGE_IMAGE:
		message = tr("Select the image series");
		break;
	case PAGE_RESULT:
		message = tr("Confirm the image information");
		break;
	default:
		message = tr("");
	}

	QMessageBox::information(this, tr("Medical Image Finder Help"), message);

}

QString MedicalImageFinder::getFileNames(unsigned int i)
{
	if (i < 0 || i >= m_numOfImages) {
		return QString();
	}
	else {
		QString fieldImagePath = "image.path" + QString::number(i);
		return field(fieldImagePath).toString();
	}
}

void MedicalImageFinder::setDirectory(QString directory)
{
	setField("directory.path", directory);
}

const QString MedicalImageFinder::getDirectory()
{
	return field("directory.path").toString();
}


int MedicalImageFinder::getNumberOfImages()
{
	return m_numOfImages;
}

