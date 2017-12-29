#include "MedicalImageFinder.h"
#include "ImagePage.h"
#include "ui_ImagePage.h"

//qt
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFileDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QThreadPool>

//itk
#include "itkGDCMSeriesFileNames.h"
#include "itkImageSeriesReader.h"
#include "itkGDCMImageIO.h"



ImagePage::ImagePage(int numOfImages, QWidget *parent) :
	QWizardPage(parent),
	ui(new Ui::ImagePage)
{
	ui->setupUi(this);
	QGridLayout* gridLayout = new QGridLayout;
	ui->verticalLayout->addLayout(gridLayout);
	// initialization 
	for (int i = 0; i < numOfImages; ++i) 
	{
		m_imageLineEdits << new QLineEdit(this);
		m_imageLineEdits[i]->setReadOnly(true);

		m_imageLineEdits2 << new QLineEdit(this);
		m_imageLineEdits2[i]->setVisible(false);

		m_imageLabels << new QLabel(this);
		m_imageLabels[i]->setBuddy(m_imageLineEdits[i]);

		m_imageSetBtns << new QPushButton("<<", this);
		m_imageSetBtns[i]->setFixedSize(30, 30);

		m_imageRemoveBtns << new QPushButton("X", this);
		m_imageRemoveBtns[i]->setFixedSize(30, 30);


		gridLayout->addWidget(m_imageLabels[i], i, 0);
		gridLayout->addWidget(m_imageLineEdits[i], i, 1);
		gridLayout->addWidget(m_imageSetBtns[i], i, 2);
		gridLayout->addWidget(m_imageRemoveBtns[i], i, 3);
		if (i == 0) {
			//* mean compulsory , otherwise optional 
			registerField("image.set" + QString::number(i) + "*", m_imageLineEdits[i]);
			registerField("image.path" + QString::number(i) + "*", m_imageLineEdits2[i]);
		}
		else {
			registerField("image.set" + QString::number(i), m_imageLineEdits[i]);
			registerField("image.path" + QString::number(i), m_imageLineEdits2[i]);
		}

		connect(m_imageSetBtns[i], SIGNAL(clicked()), this, SLOT(setImages()), Qt::UniqueConnection);
		connect(m_imageRemoveBtns[i], SIGNAL(clicked()), this, SLOT(removeImages()), Qt::UniqueConnection);
	}


	m_threadPool = new QThreadPool(this);
	m_threadPool->setExpiryTimeout(0);
}

ImagePage::ImagePage(QWidget *parent)
	:ImagePage(2, parent)
{
}

ImagePage::~ImagePage()
{
	emit finishPrevious();
	delete ui;
}

void ImagePage::setImageModalityNames(unsigned int i, QString imageModalityName)
{
	if (i < 0 || i >= static_cast<unsigned int>(m_imageLabels.size()))
		return;
	else
		m_imageLabels[i]->setText(imageModalityName);
}

bool ImagePage::validatePage()
{
	for (int i = 0; i < static_cast<unsigned int>(m_imageLabels.size()); ++i) {
		if (!m_imageLineEdits[i]->text().isEmpty())
			break;
		else if (i == m_imageLineEdits.size() - 1) {
			QMessageBox::critical(this, "No image was selected. ",
				"Please select at least one image.");
			return false;
		}
	}
	return true;
}

void ImagePage::initializePage()
{
	if (m_lastDirectory == field("directory.path").toString()) {
		return;
	}
	m_lastDirectory = field("directory.path").toString();
	//Find Images	
	QStringList allDir;
	allDir << (field("directory.path").toString());
	QDirIterator it(field("directory.path").toString(),
		QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
	while (it.hasNext())
		allDir << it.next();
	// force finish all FindImageRunnable
	emit finishPrevious();
	// setup progress bar
	ui->progressBar->setRange(0, allDir.size());
	ui->progressBar->setValue(0);
	// clean tree widget
	ui->treeWidget->clear();

	for (int i = 0; i < allDir.size(); i++)
	{
		FindImageRunnable* findImage = new FindImageRunnable(allDir[i]);
		m_threadPool->start(findImage);
		connect(findImage, SIGNAL(finish()),
			this, SLOT(updateProgressBar()));
		connect(findImage, SIGNAL(addTreeWidgetItem(QString)),
			this, SLOT(updateTreeWidget(QString)));
		connect(this, SIGNAL(finishPrevious()),
			findImage, SLOT(forceFinish()));
	}
}

int ImagePage::nextId() const
{
	return MedicalImageFinder::PAGE_RESULT;
}

void ImagePage::updateProgressBar()
{
	ui->progressBar->setValue(ui->progressBar->value() + 1);
}

void ImagePage::setImages()
{
	QPushButton* setButton = qobject_cast<QPushButton*>(sender());
	if (setButton == nullptr)
		return;

	int index = m_imageSetBtns.indexOf(setButton);
	if (index < 0 || !ui->treeWidget->currentItem())
		return;
	else {
		QTreeWidgetItem* treeWidgetItem = ui->treeWidget->currentItem();
		QString text(
			"NAME: " + treeWidgetItem->text(0) +
			"	DESCRIPTION: " + treeWidgetItem->text(2));
		m_imageLineEdits[index]->setText(text);
		m_imageLineEdits2[index]->setText(treeWidgetItem->text(7));
		//(*m_selectedImages)[index] = treeWidget->indexOfTopLevelItem(treeWidgetItem);
	}

}

void ImagePage::removeImages()
{
	QPushButton* setButton = qobject_cast<QPushButton*>(sender());
	if (setButton == nullptr)
		return;

	int index = m_imageRemoveBtns.indexOf(setButton);
	if (index < 0)
		return;
	else {
		m_imageLineEdits[index]->clear();
		m_imageLineEdits2[index]->clear();
		//(*m_selectedImages)[index] = -1;
	}


}

void ImagePage::updateTreeWidget(QString item)
{
	QTreeWidgetItem* treeWidgetItem = new QTreeWidgetItem(ui->treeWidget);
	QStringList itemList = item.split('\t');
	treeWidgetItem->setText(0, itemList[0]);
	treeWidgetItem->setText(1, itemList[1]);
	treeWidgetItem->setText(2, itemList[2]);
	treeWidgetItem->setText(3, itemList[3]);
	treeWidgetItem->setText(4, itemList[4]);
	treeWidgetItem->setText(5, itemList[5]);
	treeWidgetItem->setText(6, itemList[6]);
	treeWidgetItem->setText(7, itemList[7]);


	ui->treeWidget->addTopLevelItem(treeWidgetItem);
}


FindImageRunnable::FindImageRunnable(QString folderPath, QObject* parent)
	:m_folderPath(folderPath), QObject(parent)
{
	setAutoDelete(true);
}

void FindImageRunnable::run()
{
	using std::vector;
	using std::string;
	using itk::GDCMSeriesFileNames;
	if (m_forceFinish) {
		emit finish();
		return;
	}
	// Find Dicom files
	{
		// Generate a sequence of fileNames from a DICOM series
		GDCMSeriesFileNames::Pointer nameGenerator = GDCMSeriesFileNames::New();
		nameGenerator->SetUseSeriesDetails(true);
		nameGenerator->SetDirectory(m_folderPath.toStdString().c_str());
		//nameGenerator->AddSeriesRestriction("0008|0021");
		nameGenerator->GetInputFileNames();

		// basing on all seriesUIDs
		// get file names form corresponding serires UID
		try {
			const vector<string>& seriesUID = nameGenerator->GetSeriesUIDs();

			vector<string>::const_iterator seriesEnd = seriesUID.end();
			for (vector<string>::const_iterator cit = seriesUID.cbegin();
				cit != seriesUID.cend(); ++cit) {

				GDCImageIORead(nameGenerator->GetFileNames(*cit));

			}
		}
		catch (itk::ExceptionObject& e) {
			qDebug() << e.what();
		}

	}

	//Find Nifti file
	{
		QStringList filterList;
		filterList << "*.nii" << "*.nii.gz";
		QDir dir(m_folderPath);
		QFileInfoList niiFileList = dir.entryInfoList(filterList);

		// basing on wildcard to find out all NIFTI files
		for (QFileInfoList::const_iterator cit = niiFileList.cbegin();
			cit != niiFileList.cend(); ++cit)
		{
			vector<string> fileNames(1, cit->absoluteFilePath().toStdString());
			GDCImageIORead(fileNames);
		}
	}
	emit finish();
}

void FindImageRunnable::forceFinish()
{
	m_forceFinish = true;
	disconnect(0, 0, 0);
}

void FindImageRunnable::GDCImageIORead(const std::vector<std::string>& files)
{
	using std::string;
	using std::vector;
	using itk::GDCMImageIO;
	typedef itk::ImageFileReader<itk::Image<unsigned short, 2>> ImageFileReader;

	string _file = files[0];
	string patientName = "/";
	string modality = "/";
	string seriesDescription = "/";
	string slice = "/";
	string date = "/";
	string seriesUID = "/";
	string format = "/";

	try {
		GDCMImageIO::Pointer dicomIO = GDCMImageIO::New();

		ImageFileReader::Pointer imageFilerReader = ImageFileReader::New();
		imageFilerReader->SetImageIO(dicomIO);
		imageFilerReader->SetFileName(_file);
		imageFilerReader->Update();

		if (!dicomIO->GetValueFromTag(("0010|0010"), patientName)) {
			//patientName = "/";
		}
		if (!dicomIO->GetValueFromTag("0008|0060", modality)) {
			//modality = "/";
		}
		if (!dicomIO->GetValueFromTag("0008|103e", seriesDescription)) {
			//seriesDescription = "/";
		}

		if (!dicomIO->GetValueFromTag("0020|1002", slice)) {
			//slice = "/";
			slice = std::to_string(files.size());
		}
		if (!dicomIO->GetValueFromTag("0008|0020", date)) {
			//date = "/";
		}
		if (!dicomIO->GetValueFromTag("0020|0052", seriesUID)) {
			//seriesUID = "/";
		}
	}
	catch (itk::ExceptionObject& e) {
		patientName = QFileInfo(QString::fromStdString(_file)).baseName().toStdString();
		qDebug() << "Input are not DICOM images.";
		qDebug() << "Input are assumed to be NIFTI.";
		qDebug() << e.what();

	}

	QString _emitString;
	_emitString += QString::fromStdString(patientName) + '\t';
	_emitString += QString::fromStdString(modality) + '\t';
	_emitString += QString::fromStdString(seriesDescription) + '\t';
	_emitString += QString::fromStdString(slice) + '\t';
	_emitString += QString::fromStdString(date) + '\t';
	_emitString += QString::fromStdString(seriesUID) + '\t';
	_emitString += QString::fromStdString(format) + '\t';

	for (vector<string>::const_iterator cit = files.cbegin();
		cit != files.cend(); ++cit) {
		_emitString += QString::fromStdString(*cit);
		_emitString += ";";   //RegistrationWizard::SEPERATOR;
	}
	_emitString.remove(_emitString.size() - 1, 1);
	qDebug() << _emitString;

	emit addTreeWidgetItem(_emitString);
}