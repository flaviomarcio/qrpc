#pragma once

#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDebug>
#include <QStringList>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QLocale>
#include <QJsonDocument>
#include <gtest/gtest.h>
#include "../../qstm/src/qstm_types.h"

namespace QRpc {

class SDKGoogleTest : public testing::Test{
public:

    //!
    //! \brief SDKGoogleTest
    //!
    explicit SDKGoogleTest();

    //!
    //! \brief SetUpTestCase
    //!
    static void SetUpTestCase()
    {
    }

    //!
    //! \brief SetUp
    //!
    virtual void SetUp()
    {
    }

    //!
    //! \brief TearDown
    //!
    virtual void TearDown()
    {
    }

    //!
    //! \brief TearDownTestCase
    //!
    static void TearDownTestCase()
    {
    }

    //!
    //! \brief clear
    //! \return
    //!
    virtual bool clear();

    //!
    //! \brief arguments
    //! \return
    //!
    virtual QStringList arguments()const;

    //!
    //! \brief toMd5
    //! \param v
    //! \return
    //!
    static const QByteArray toMd5(const QVariant &v);

    //!
    //! \brief toVar
    //! \param v
    //! \return
    //!
    static const QVariant toVar(const QVariant &v);

    //!
    //! \brief serviceStart
    //! \return
    //!
    virtual bool serviceStart();

    //!
    //! \brief serviceStop
    //! \return
    //!
    virtual bool serviceStop();

public:

};

}


