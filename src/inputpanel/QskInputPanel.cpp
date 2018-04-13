/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskInputPanel.h"
#include "QskVirtualKeyboard.h"
#include "QskInputSuggestionBar.h"
#include "QskLinearBox.h"

#include <QString>
#include <QLocale>
#include <QGuiApplication>

QString qskNativeLocaleString( const QLocale& locale )
{
    switch( locale.language() )
    {
        case QLocale::Bulgarian:
            return QStringLiteral( "български език" );

        case QLocale::Czech:
            return QStringLiteral( "Čeština" );

        case QLocale::German:
            return QStringLiteral( "Deutsch" );

        case QLocale::Danish:
            return QStringLiteral( "Dansk" );

        case QLocale::Greek:
            return QStringLiteral( "Eλληνικά" );

        case QLocale::English:
        {
            switch( locale.country() )
            {
                case QLocale::Canada:
                case QLocale::UnitedStates:
                case QLocale::UnitedStatesMinorOutlyingIslands:
                case QLocale::UnitedStatesVirginIslands:
                    return QStringLiteral( "English (US)" );

                default:
                    return QStringLiteral( "English (UK)" );
            }
        }

        case QLocale::Spanish:
            return QStringLiteral( "Español" );

        case QLocale::Finnish:
            return QStringLiteral( "Suomi" );

        case QLocale::French:
            return QStringLiteral( "Français" );

        case QLocale::Hungarian:
            return QStringLiteral( "Magyar" );

        case QLocale::Italian:
            return QStringLiteral( "Italiano" );

        case QLocale::Japanese:
            return QStringLiteral( "日本語" );

        case QLocale::Latvian:
            return QStringLiteral( "Latviešu" );

        case QLocale::Lithuanian:
            return QStringLiteral( "Lietuvių" );

        case QLocale::Dutch:
            return QStringLiteral( "Nederlands" );

        case QLocale::Portuguese:
            return QStringLiteral( "Português" );

        case QLocale::Romanian:
            return QStringLiteral( "Română" );

        case QLocale::Russia:
            return QStringLiteral( "Русский" );

        case QLocale::Slovenian:
            return QStringLiteral( "Slovenščina" );

        case QLocale::Slovak:
            return QStringLiteral( "Slovenčina" );

        case QLocale::Turkish:
            return QStringLiteral( "Türkçe" );

        case QLocale::Chinese:
            return QStringLiteral( "中文" );

        default:
            return QLocale::languageToString( locale.language() );
    }
}

class QskInputPanel::PrivateData
{
public:
    QskLinearBox* layout;
    QskInputSuggestionBar* suggestionBar;
    QskVirtualKeyboard* keyboard;
};

QskInputPanel::QskInputPanel( QQuickItem* parent ):
    QskControl( parent ),
    m_data( new PrivateData() )
{
    setAutoLayoutChildren( true );
    initSizePolicy( QskSizePolicy::Expanding, QskSizePolicy::Constrained );

    m_data->layout = new QskLinearBox( Qt::Vertical, this );

    m_data->suggestionBar = new QskInputSuggestionBar( m_data->layout );
    m_data->suggestionBar->setVisible( false );

    connect( m_data->suggestionBar, &QskInputSuggestionBar::suggested,
        this, &QskInputPanel::commitCandidate );

    m_data->keyboard = new QskVirtualKeyboard( m_data->layout );

    connect( m_data->keyboard, &QskVirtualKeyboard::keySelected,
        this, &QskInputPanel::commitKey );
}

QskInputPanel::~QskInputPanel()
{
}

qreal QskInputPanel::heightForWidth( qreal width ) const
{
    /*
        This code looks like as it could be generalized
        and moved to QskLinearBox. TODO ...
     */

    const auto margins = this->margins();

    width -= margins.left() + margins.right();

    qreal height = m_data->keyboard->heightForWidth( width );

    if ( m_data->suggestionBar->isVisible() )
    {
        height += m_data->layout->spacing();
        height += m_data->suggestionBar->sizeHint().height();
    }

    height += margins.top() + margins.bottom();

    return height;
}

qreal QskInputPanel::widthForHeight( qreal height ) const
{
    const auto margins = this->margins();

    height -= margins.top() + margins.bottom();

    if ( m_data->suggestionBar->isVisible() )
    {
        height -= m_data->layout->spacing();
        height -= m_data->suggestionBar->sizeHint().height();
    }

    qreal width = m_data->keyboard->widthForHeight( height );
    width += margins.left() + margins.right();

    return width;
}

bool QskInputPanel::isCandidatesEnabled() const
{
    return m_data->suggestionBar->isVisible();
}

QVector< QString > QskInputPanel::candidates() const
{
    return m_data->suggestionBar->candidates();
}

void QskInputPanel::setCandidatesEnabled( bool on )
{
    m_data->suggestionBar->setVisible( on );
}

void QskInputPanel::setCandidates( const QVector< QString >& candidates )
{
    m_data->suggestionBar->setCandidates( candidates );
}

void QskInputPanel::commitCandidate( int index )
{
    m_data->suggestionBar->setCandidates( QVector< QString >() );

    QGuiApplication::inputMethod()->invokeAction(
        static_cast< QInputMethod::Action >( SelectCandidate ), index );
}

void QskInputPanel::commitKey( int key )
{
    QGuiApplication::inputMethod()->invokeAction(
        static_cast< QInputMethod::Action >( Compose ), key );
}

void QskInputPanel::keyPressEvent( QKeyEvent* event )
{
    // animate the corresponding key button TODO

    switch( event->key() )
    {
        case Qt::Key_Return:
        case Qt::Key_Escape:
        {
            commitKey( event->key() );
            break;
        }

        default:
        {
            const auto text = event->text();
            if ( !text.isEmpty() )
                commitKey( text[0].unicode() );
            else
                commitKey( event->key() );
        }
    }
}

void QskInputPanel::keyReleaseEvent( QKeyEvent* event )
{
    return Inherited::keyReleaseEvent( event );
}

#include "moc_QskInputPanel.cpp"
