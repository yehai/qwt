/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include "qwt_column_symbol.h"
#include "qwt_math.h"
#include "qwt_text.h"
#include "qwt_painter.h"
#include <qpainter.h>
#include <qpalette.h>

static void drawBox(QPainter *p, const QRectF &rect,
    const QPalette &pal, double lw)
{
    if ( lw > 0.0 )
    {
        if ( rect.width() == 0.0 )
        {
            p->setPen(pal.dark().color());
            p->drawLine(rect.topLeft(), rect.bottomLeft());
            return;
        }

        if ( rect.height() == 0.0 )
        {
            p->setPen(pal.dark().color());
            p->drawLine(rect.topLeft(), rect.topRight());
            return;
        }

        lw = qMin(lw, rect.height() / 2.0 - 1.0);
        lw = qMin(lw, rect.width() / 2.0 - 1.0);

        const QRectF outerRect = rect.adjusted(0, 0, 1, 1);
        QPolygonF polygon(outerRect);

        if ( outerRect.width() > 2 * lw && 
            outerRect.height() > 2 * lw )
        {
            const QRectF innerRect = outerRect.adjusted(lw, lw, -lw, -lw);
            polygon = polygon.subtracted(innerRect);
        }

        p->setPen(Qt::NoPen);

        p->setBrush(pal.dark());
        p->drawPolygon(polygon);
    }

    const QRectF windowRect = rect.adjusted(lw, lw, -lw + 1, -lw + 1);
    if ( windowRect.isValid() )
        p->fillRect(windowRect, pal.window());
}

static void drawPanel(QPainter *p, const QRectF &rect,
    const QPalette &pal, double lw)
{

    if ( lw > 0.0 )
    {
        if ( rect.width() == 0.0 )
        {
            p->setPen(pal.window().color());
            p->drawLine(rect.topLeft(), rect.bottomLeft());
            return;
        }

        if ( rect.height() == 0.0 )
        {
            p->setPen(pal.window().color());
            p->drawLine(rect.topLeft(), rect.topRight());
            return;
        }

        lw = qMin(lw, rect.height() / 2.0 - 1.0);
        lw = qMin(lw, rect.width() / 2.0 - 1.0);

        const QRectF outerRect = rect.adjusted(0, 0, 1, 1);
        const QRectF innerRect = outerRect.adjusted(lw, lw, -lw, -lw);

        QPolygonF lines[2];

        lines[0] += outerRect.bottomLeft();
        lines[0] += outerRect.topLeft();
        lines[0] += outerRect.topRight();
        lines[0] += innerRect.topRight();
        lines[0] += innerRect.topLeft();
        lines[0] += innerRect.bottomLeft();

        lines[1] += outerRect.topRight();
        lines[1] += outerRect.bottomRight();
        lines[1] += outerRect.bottomLeft();
        lines[1] += innerRect.bottomLeft();
        lines[1] += innerRect.bottomRight();
        lines[1] += innerRect.topRight();

        p->setPen(Qt::NoPen);

        p->setBrush(pal.light());
        p->drawPolygon(lines[0]);
        p->setBrush(pal.dark());
        p->drawPolygon(lines[1]);
    }

    p->fillRect(rect.adjusted(lw, lw, -lw + 1, -lw + 1), pal.window());
}

class QwtColumnSymbol::PrivateData
{
public:
    PrivateData():
        style(QwtColumnSymbol::Box),
        frameStyle(QwtColumnSymbol::Raised),
        lineWidth(2)
    {
        palette = QPalette(Qt::gray);
    }

    QwtColumnSymbol::Style style;
    QwtColumnSymbol::FrameStyle frameStyle;

    QPalette palette;
    QwtText label;

    int lineWidth;
};

QwtColumnSymbol::QwtColumnSymbol(Style style) 
{
    d_data = new PrivateData();
    d_data->style = style;
}

QwtColumnSymbol::~QwtColumnSymbol()
{
    delete d_data;
}

//! == operator
bool QwtColumnSymbol::operator==(const QwtColumnSymbol &other) const
{
    return d_data->style == other.d_data->style &&
        d_data->palette == other.d_data->palette &&
        d_data->label == other.d_data->label &&
        d_data->lineWidth == other.d_data->lineWidth &&
        d_data->frameStyle == other.d_data->frameStyle;
}

//! != operator
bool QwtColumnSymbol::operator!=(const QwtColumnSymbol &other) const
{
    return !(*this == other);
}

void QwtColumnSymbol::setStyle(Style style)
{
    d_data->style = style;
}

QwtColumnSymbol::Style QwtColumnSymbol::style() const
{
    return d_data->style;
}

void QwtColumnSymbol::setPalette(const QPalette &palette)
{
    d_data->palette = palette;
}

const QPalette& QwtColumnSymbol::palette() const
{
    return d_data->palette;
}

void QwtColumnSymbol::setFrameStyle(FrameStyle style)
{
    d_data->frameStyle = style;
}

QwtColumnSymbol::FrameStyle QwtColumnSymbol::frameStyle() const
{
    return d_data->frameStyle;
}

void QwtColumnSymbol::setLineWidth(int width)
{
    d_data->lineWidth = width;
}

int QwtColumnSymbol::lineWidth() const
{
    return d_data->lineWidth;
}


void QwtColumnSymbol::setLabel(const QwtText &label)
{
    d_data->label = label;
}

const QwtText& QwtColumnSymbol::label() const
{
    return d_data->label;
}

void QwtColumnSymbol::draw(QPainter *painter, 
    const QwtColumnRect &rect) const
{
    painter->save();

    switch(d_data->style)
    {
        case QwtColumnSymbol::Box:
        {
            drawBox(painter, rect);
            break;
        }
        default:;
    }

    painter->restore();
}

void QwtColumnSymbol::drawBox(QPainter *painter, 
    const QwtColumnRect &rect) const
{
    QRectF r = rect.toRect();
    if ( rect.hInterval.borderFlags() & QwtDoubleInterval::ExcludeMinimum )
        r.adjust(1, 0, 0, 0);
    if ( rect.hInterval.borderFlags() & QwtDoubleInterval::ExcludeMaximum )
        r.adjust(0, 0, -1, 0);
    if ( rect.vInterval.borderFlags() & QwtDoubleInterval::ExcludeMinimum )
        r.adjust(0, 1, 0, 0);
    if ( rect.vInterval.borderFlags() & QwtDoubleInterval::ExcludeMaximum )
        r.adjust(0, 0, 0, -1);

    switch(d_data->frameStyle) 
    {
        case QwtColumnSymbol::Raised:
        {
            ::drawPanel(painter, r, d_data->palette, d_data->lineWidth);
            break;
        }
        case QwtColumnSymbol::Plain:
        {
            ::drawBox(painter, r, d_data->palette, d_data->lineWidth);
            break;
        }
        default:
        {
            painter->fillRect(r.adjusted(0, 0, -1, -1),
                d_data->palette.window());
        }
    }
}