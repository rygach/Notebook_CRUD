#ifndef PDFCONVERTER_H
#define PDFCONVERTER_H

#include <QObject>
#include <QString>

class PdfConverter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isConverting READ isConverting NOTIFY isConvertingChanged)
    
public:
    explicit PdfConverter(QObject *parent = nullptr);
    
    // Основной метод конвертации
    Q_INVOKABLE bool convertMarkdownToPdf(const QString &mdFilePath, 
                                           const QString &pdfFilePath);
    
    // Свойство для отслеживания процесса
    bool isConverting() const { return m_isConverting; }
    
signals:
    void conversionCompleted(bool success, const QString &message);
    void isConvertingChanged();
    
private:
    // Конвертация Markdown в HTML
    QString markdownToHtml(const QString &markdown);
    
    // Вспомогательные методы парсинга
    QString parseHeaders(const QString &line);
    QString parseBold(const QString &text);
    QString parseItalic(const QString &text);
    QString parseCode(const QString &text);
    QString parseLinks(const QString &text);
    QString parseCodeBlock(const QString &block);
    QString parseList(const QString &line);
    
    void setIsConverting(bool converting);
    
    bool m_isConverting;
};

#endif // PDFCONVERTER_H

