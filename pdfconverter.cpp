#include "pdfconverter.h"
#include <QFile>
#include <QTextStream>
#include <QTextDocument>
#include <QPdfWriter>
#include <QPageSize>
#include <QDebug>
#include <QRegularExpression>

PdfConverter::PdfConverter(QObject *parent)
    : QObject(parent)
    , m_isConverting(false)
{
}

bool PdfConverter::convertMarkdownToPdf(const QString &mdFilePath, const QString &pdfFilePath)
{
    setIsConverting(true);
    
    // Удаляем file:/// префикс если есть
    QString mdPath = mdFilePath;
    QString pdfPath = pdfFilePath;
    
    if (mdPath.startsWith("file:///")) {
        mdPath = mdPath.mid(8);
    }
    if (pdfPath.startsWith("file:///")) {
        pdfPath = pdfPath.mid(8);
    }
    
    qDebug() << "Converting MD to PDF:";
    qDebug() << "  MD file:" << mdPath;
    qDebug() << "  PDF file:" << pdfPath;
    
    // Читаем Markdown файл
    QFile mdFile(mdPath);
    if (!mdFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString error = QString("Cannot open Markdown file: %1").arg(mdFile.errorString());
        qWarning() << error;
        emit conversionCompleted(false, error);
        setIsConverting(false);
        return false;
    }
    
    QTextStream in(&mdFile);
    in.setCodec("UTF-8");
    QString markdownContent = in.readAll();
    mdFile.close();
    
    if (markdownContent.isEmpty()) {
        QString error = "Markdown file is empty";
        qWarning() << error;
        emit conversionCompleted(false, error);
        setIsConverting(false);
        return false;
    }
    
    // Конвертируем Markdown в HTML
    QString htmlContent = markdownToHtml(markdownContent);
    
    // Создаем PDF
    QPdfWriter pdfWriter(pdfPath);
    pdfWriter.setPageSize(QPageSize(QPageSize::A4));
    pdfWriter.setPageMargins(QMarginsF(20, 20, 20, 20));
    pdfWriter.setResolution(300);
    
    // Создаем текстовый документ и загружаем HTML
    QTextDocument document;
    document.setHtml(htmlContent);
    document.setPageSize(pdfWriter.pageLayout().paintRectPixels(pdfWriter.resolution()).size());
    
    // Экспортируем в PDF
    document.print(&pdfWriter);
    
    qDebug() << "PDF created successfully:" << pdfPath;
    
    emit conversionCompleted(true, QString("PDF created successfully: %1").arg(pdfPath));
    setIsConverting(false);
    
    return true;
}

QString PdfConverter::markdownToHtml(const QString &markdown)
{
    QString html = "<!DOCTYPE html>\n<html>\n<head>\n";
    html += "<meta charset=\"UTF-8\">\n";
    html += "<style>\n";
    html += "body { font-family: 'Segoe UI', Arial, sans-serif; line-height: 1.6; }\n";
    html += "h1 { color: #2c3e50; border-bottom: 2px solid #3498db; padding-bottom: 10px; }\n";
    html += "h2 { color: #34495e; border-bottom: 1px solid #bdc3c7; padding-bottom: 8px; }\n";
    html += "h3 { color: #555; }\n";
    html += "code { background-color: #f4f4f4; padding: 2px 6px; border-radius: 3px; font-family: 'Courier New', monospace; }\n";
    html += "pre { background-color: #f4f4f4; padding: 15px; border-radius: 5px; overflow-x: auto; }\n";
    html += "pre code { background-color: transparent; padding: 0; }\n";
    html += "blockquote { border-left: 4px solid #3498db; padding-left: 15px; color: #555; font-style: italic; }\n";
    html += "table { border-collapse: collapse; width: 100%; margin: 20px 0; }\n";
    html += "th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }\n";
    html += "th { background-color: #3498db; color: white; }\n";
    html += "ul, ol { padding-left: 30px; }\n";
    html += "a { color: #3498db; text-decoration: none; }\n";
    html += "a:hover { text-decoration: underline; }\n";
    html += "</style>\n";
    html += "</head>\n<body>\n";
    
    QStringList lines = markdown.split('\n');
    bool inCodeBlock = false;
    QString codeBlockContent;
    bool inTable = false;
    
    for (int i = 0; i < lines.size(); i++) {
        QString line = lines[i];
        
        // Обработка code blocks
        if (line.startsWith("```")) {
            if (!inCodeBlock) {
                inCodeBlock = true;
                codeBlockContent.clear();
                continue;
            } else {
                inCodeBlock = false;
                html += parseCodeBlock(codeBlockContent);
                codeBlockContent.clear();
                continue;
            }
        }
        
        if (inCodeBlock) {
            codeBlockContent += line + "\n";
            continue;
        }
        
        // Пустые строки
        if (line.trimmed().isEmpty()) {
            html += "<br>\n";
            continue;
        }
        
        // Заголовки
        QString headerResult = parseHeaders(line);
        if (!headerResult.isEmpty()) {
            html += headerResult + "\n";
            continue;
        }
        
        // Списки
        QString listResult = parseList(line);
        if (!listResult.isEmpty()) {
            html += listResult + "\n";
            continue;
        }
        
        // Таблицы
        if (line.contains("|")) {
            if (!inTable) {
                html += "<table>\n";
                inTable = true;
            }
            
            QStringList cells = line.split("|", QString::SkipEmptyParts);
            bool isHeader = (i + 1 < lines.size() && lines[i + 1].contains("---"));
            
            if (isHeader) {
                html += "<thead><tr>\n";
                for (const QString &cell : cells) {
                    html += QString("<th>%1</th>\n").arg(cell.trimmed());
                }
                html += "</tr></thead>\n<tbody>\n";
            } else if (!lines[i - 1].contains("---")) {
                html += "<tr>\n";
                for (const QString &cell : cells) {
                    html += QString("<td>%1</td>\n").arg(cell.trimmed());
                }
                html += "</tr>\n";
            }
            continue;
        } else if (inTable) {
            html += "</tbody></table>\n";
            inTable = false;
        }
        
        // Обычный текст с форматированием
        QString processedLine = line;
        processedLine = parseBold(processedLine);
        processedLine = parseItalic(processedLine);
        processedLine = parseCode(processedLine);
        processedLine = parseLinks(processedLine);
        
        html += "<p>" + processedLine + "</p>\n";
    }
    
    if (inTable) {
        html += "</tbody></table>\n";
    }
    
    html += "</body>\n</html>";
    
    return html;
}

QString PdfConverter::parseHeaders(const QString &line)
{
    if (line.startsWith("# ")) {
        return QString("<h1>%1</h1>").arg(line.mid(2).trimmed());
    } else if (line.startsWith("## ")) {
        return QString("<h2>%1</h2>").arg(line.mid(3).trimmed());
    } else if (line.startsWith("### ")) {
        return QString("<h3>%1</h3>").arg(line.mid(4).trimmed());
    } else if (line.startsWith("#### ")) {
        return QString("<h4>%1</h4>").arg(line.mid(5).trimmed());
    }
    return QString();
}

QString PdfConverter::parseBold(const QString &text)
{
    QString result = text;
    
    // **bold** или __bold__
    QRegularExpression boldRegex("\\*\\*(.+?)\\*\\*");
    result.replace(boldRegex, "<strong>\\1</strong>");
    
    QRegularExpression boldRegex2("__(.+?)__");
    result.replace(boldRegex2, "<strong>\\1</strong>");
    
    return result;
}

QString PdfConverter::parseItalic(const QString &text)
{
    QString result = text;
    
    // *italic* или _italic_ (но не ** или __)
    QRegularExpression italicRegex("(?<!\\*)\\*(?!\\*)(.+?)(?<!\\*)\\*(?!\\*)");
    result.replace(italicRegex, "<em>\\1</em>");
    
    QRegularExpression italicRegex2("(?<!_)_(?!_)(.+?)(?<!_)_(?!_)");
    result.replace(italicRegex2, "<em>\\1</em>");
    
    return result;
}

QString PdfConverter::parseCode(const QString &text)
{
    QString result = text;
    
    // `code`
    QRegularExpression codeRegex("`(.+?)`");
    result.replace(codeRegex, "<code>\\1</code>");
    
    return result;
}

QString PdfConverter::parseLinks(const QString &text)
{
    QString result = text;
    
    // [text](url)
    QRegularExpression linkRegex("\\[(.+?)\\]\\((.+?)\\)");
    result.replace(linkRegex, "<a href=\"\\2\">\\1</a>");
    
    return result;
}

QString PdfConverter::parseCodeBlock(const QString &block)
{
    QString escaped = block;
    escaped.replace("<", "&lt;");
    escaped.replace(">", "&gt;");
    
    return QString("<pre><code>%1</code></pre>\n").arg(escaped);
}

QString PdfConverter::parseList(const QString &line)
{
    // Нумерованные списки: 1. item
    QRegularExpression orderedListRegex("^\\d+\\.\\s+(.+)$");
    QRegularExpressionMatch orderedMatch = orderedListRegex.match(line);
    if (orderedMatch.hasMatch()) {
        return QString("<li>%1</li>").arg(orderedMatch.captured(1));
    }
    
    // Ненумерованные списки: - item или * item
    if (line.trimmed().startsWith("- ") || line.trimmed().startsWith("* ")) {
        return QString("<li>%1</li>").arg(line.trimmed().mid(2));
    }
    
    return QString();
}

void PdfConverter::setIsConverting(bool converting)
{
    if (m_isConverting != converting) {
        m_isConverting = converting;
        emit isConvertingChanged();
    }
}

