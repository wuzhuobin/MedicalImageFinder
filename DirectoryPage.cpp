#include "MedicalImageFinder.h"
#include "DirectoryPage.h"
#include "ui_DirectoryPage.h"

//qt
#include <QDir>
#include <QFileDialog>

DirectoryPage::DirectoryPage(QString dir, QWidget *parent) :
	QWizardPage(parent),
	ui(new Ui::DirectoryPage)
{
	ui->setupUi(this);
	registerField("directory.path", ui->lineEdit_directory);
	// not working?
	//setField("directory.path", dir);
	ui->lineEdit_directory->setText(dir);
	

	connect(ui->pushButtonDirectoryBrowse, SIGNAL(clicked()), this, SLOT(browse()));
}

DirectoryPage::DirectoryPage(QWidget *parent)
	: DirectoryPage(QString(), parent)
{
}

DirectoryPage::~DirectoryPage()
{
	delete ui;
}

void DirectoryPage::browse()
{
	QDir dir = QDir(ui->lineEdit_directory->text());
	if (!dir.exists()) {
		dir = QDir();
	}

	//Dialog Open Directory
	QString path = QFileDialog::getExistingDirectory(this,
		tr("Open Image Directory"), dir.absolutePath(),
		QFileDialog::ShowDirsOnly);
	path.replace("\\", "/");
	ui->lineEdit_directory->setText(path);
}

int DirectoryPage::nextId() const
{
	return MedicalImageFinder::PAGE_IMAGE;
}