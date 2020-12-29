#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QWidget>

namespace Ui {
class ImageView;
}

class ImageView : public QWidget
{
    Q_OBJECT

public:
    explicit ImageView(QWidget *parent = nullptr);
    ~ImageView();

	void SetQssSheetStyle(QString sheet_style);
	void SetTitle(QString title);
protected:
	virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;
private:
	void InitializeUI();
	void ReleasePointer();
	bool CreateBorderEvent(const QByteArray &eventType,void *message,long *result);

    Ui::ImageView *ui;
};

#endif // IMAGEVIEW_H
