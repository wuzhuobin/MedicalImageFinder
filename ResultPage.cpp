#include "MedicalImageFinder.h"
#include "ResultPage.h"
#include "ui_ResultPage.h"

//qt
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QTableWidget>

//itk
#include "itkGDCMSeriesFileNames.h"
#include "itkImageSeriesReader.h"
#include "itkGDCMImageIO.h"

ResultPage::ResultPage(int numOfImages, QWidget *parent):
	QWizardPage(parent),
	ui(new Ui::ResultPage)
{
	ui->setupUi(this);
	for (int i = 0; i < numOfImages; ++i) {
		m_tableWidgets << nullptr;
	}
}

ResultPage::ResultPage(QWidget *parent)
	: ResultPage(2, parent)
{
}

ResultPage::~ResultPage()
{
	delete ui;
}

int ResultPage::nextId() const
{
	return -1;
}

void ResultPage::initializePage()
{
	using itk::GDCMImageIO;
	using itk::MetaDataDictionary;
	using std::string;
	typedef itk::ImageFileReader<itk::Image<unsigned short, 2>> ImageFileReader;
	using itk::MetaDataObjectBase;
	typedef itk::MetaDataObject<string> MetaDataString;


	ui->tabWidget->clear();


	for (int i = 0; i < m_tableWidgets.size(); ++i) {

		QString fieldStringImageSet;
		fieldStringImageSet += "image.set";
		fieldStringImageSet += QString::number(i);

		QString imageSet = field(fieldStringImageSet).toString();

		if (imageSet.isEmpty()) {
			continue;
		}

		if (m_tableWidgets[i] == nullptr) {
			m_tableWidgets[i] = new QTableWidget(this);
		}
		ui->tabWidget->addTab(m_tableWidgets[i], imageSet);
		//m_tableWidgets[i]->clear();
		m_tableWidgets[i]->setEditTriggers(QTableWidget::NoEditTriggers);
		m_tableWidgets[i]->setColumnCount(3);
		m_tableWidgets[i]->setRowCount(0);
		m_tableWidgets[i]->setHorizontalHeaderLabels(QStringList()
			<< "Tag" << "Label" << "Description");
		m_tableWidgets[i]->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
		m_tableWidgets[i]->verticalHeader()->setVisible(false);

		try {
			GDCMImageIO::Pointer dicomIO = GDCMImageIO::New();
			ImageFileReader::Pointer reader = ImageFileReader::New();

			QString fieldStringImagePath;
			fieldStringImagePath += "image.path";
			fieldStringImagePath += QString::number(i);

			QStringList imagePaths = field(fieldStringImagePath).toString().split(MedicalImageFinder::SEPERATOR);
			if (imagePaths[0].isEmpty()) {
				continue;
			}

			reader->SetFileName(imagePaths[0].toStdString());
			qDebug() << imagePaths;
			reader->SetImageIO(dicomIO);
			reader->Update();

			const MetaDataDictionary& dictionary = dicomIO->GetMetaDataDictionary();
			for (MetaDataDictionary::ConstIterator cit = dictionary.Begin();
				cit != dictionary.End(); ++cit) {

				MetaDataObjectBase::Pointer entry = cit->second;
				MetaDataString::Pointer entryValue = static_cast<MetaDataString*>(
					entry.GetPointer());

				if (entryValue) {
					m_tableWidgets[i]->insertRow(m_tableWidgets[i]->rowCount());
					string tag = cit->first;
					string label;
					string value = entryValue->GetMetaDataObjectValue();
					m_tableWidgets[i]->setItem(m_tableWidgets[i]->rowCount() - 1, 2,
						new QTableWidgetItem(QString::fromStdString(value)));
					m_tableWidgets[i]->setItem(m_tableWidgets[i]->rowCount() - 1, 0,
						new QTableWidgetItem(QString::fromStdString(tag)));
					if (GDCMImageIO::GetLabelFromTag(tag, label)) {
						m_tableWidgets[i]->setItem(m_tableWidgets[i]->rowCount() - 1, 1,
							new QTableWidgetItem(QString::fromStdString(label)));
					}
					else {
						m_tableWidgets[i]->setItem(m_tableWidgets[i]->rowCount() - 1, 1,
							new QTableWidgetItem(tr("Unknown")));
					}

				}
			}
		}
		catch (itk::ExceptionObject& e) {
			qDebug() << "Inputs are not DICOM images.";
			qDebug() << e.what();
		}

		//if (!(*m_DICOMHeaders)[i].isNull()) {
		//	int tableRow = 0;
		//	m_tableWidgets[i]->setRowCount((*m_DICOMHeaders).size());
		//	m_tableWidgets[i]->setColumnCount(2);
		//	for (QMap<QString, QString>::const_iterator cit = (*m_DICOMHeaders)[i]->constBegin();
		//		cit != (*m_DICOMHeaders)[i]->constEnd(); ++cit) {

		//		qDebug() << cit.key();
		//		qDebug() << cit.value();
		//		m_tableWidgets[i]->setItem(tableRow, 0, new QTableWidgetItem(cit.key()));
		//		m_tableWidgets[i]->setItem(tableRow, 1, new QTableWidgetItem(cit.value()));
		//		++tableRow;
		//	}
		//}

	}

}