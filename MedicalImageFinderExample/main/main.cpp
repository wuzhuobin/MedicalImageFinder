//#include "RegistrationWizard.h"
#include "MedicalImageFinder.h"
#include <qdebug.h>
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	//RegistrationWizard rw("E:/TestProgram/git/RegistrationWizard/TestImage", 2);
	MedicalImageFinder rw("E:/TestProgram/git/RegistrationWizard/TestImage", 2);
	rw.setImageModalityNames(0, "T2 image ");
	rw.setImageModalityNames(1, "MRA image");

	
	if (QWizard::Accepted == rw.exec()) {


		qDebug() << rw.getFileNames(0);
		qDebug() << rw.getFileNames(1);
		qDebug() << rw.getDirectory();

	}
	return a.exec();
}
