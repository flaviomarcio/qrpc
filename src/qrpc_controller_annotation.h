#pragma once

#include <QObject>
#include <QVariantHash>
#include <QVariantList>
#include <QString>
#include "../../qannotation/src/qannotation_extended.h"
#include "../../qannotation/src/qannotation_macros.h"
#include "./qrpc_global.h"

namespace QRpcPrivate
{

//!
//! \brief The AnotationsExtended class
//!
class Q_RPC_EXPORT AnotationsExtended:public QAnnotation::Extended
{
public:

    enum Classification{
        ApiOperation
    };

    //!
    //! \brief AnotationsExtended
    //! \param parent
    //!
    explicit AnotationsExtended(QObject *parent=nullptr);

    //!
    //! \brief apiDoc
    //!
    Q_ANNOTATION_DECLARE(apiDoc, Documentation)

    //!
    //! \brief apiName
    //!
    Q_ANNOTATION_DECLARE_FUNC(apiName, Information)

    //!
    //! \brief apiDescription
    //!
    Q_ANNOTATION_DECLARE_FUNC(apiDescription, Information)

    //!
    //! \brief apiGroup
    //!
    Q_ANNOTATION_DECLARE_FUNC(apiGroup, Information)

    //!
    //! \brief apiModule
    //!
    Q_ANNOTATION_DECLARE_FUNC(apiModule, Information)

    //!
    //! \brief apiNameOrder
    //!
    Q_ANNOTATION_DECLARE_FUNC(apiNameOrder, Information)

    //!
    //! \brief apiOrder
    //!
    Q_ANNOTATION_DECLARE_FUNC(apiOrder, Information)

    //!
    //! \brief apiRedirect
    //!
    Q_ANNOTATION_DECLARE(apiRedirect, Information)

    //!
    //! \brief apiBasePath
    //!
    Q_ANNOTATION_DECLARE_FUNC(apiBasePath, Information)

    //!
    //! \brief apiCRUDDesciptors
    //!
    Q_ANNOTATION_DECLARE(apiCRUDDesciptors, Information)

    //!
    //!
    Q_ANNOTATION_DECLARE_FUNC(opName, Information)

    //!
    //!
    Q_ANNOTATION_DECLARE_FUNC(opOrder, Information)

    //!
    //! \brief opDescription
    //!
    Q_ANNOTATION_DECLARE_FUNC(opDescription, Information)

    //!
    //! \brief opGroup
    //!
    Q_ANNOTATION_DECLARE_FUNC(opGroup, Information)

    //!
    //! \brief opPath
    //! \param path
    //! \return
    //!
    Q_ANNOTATION_DECLARE_FUNC(opPath, Information)

    //!
    //! \brief opCrud
    //!
    Q_ANNOTATION_DECLARE(opCrud, ApiOperation)

    //!
    //! \brief opTrace
    //!
    Q_ANNOTATION_DECLARE(opTrace, ApiOperation)

    //!
    //! \brief opPatch
    //!
    Q_ANNOTATION_DECLARE(opPatch, ApiOperation)

    //!
    //! \brief opHead
    //!
    Q_ANNOTATION_DECLARE(opHead, ApiOperation)

    //!
    //! \brief opOptions
    //!
    Q_ANNOTATION_DECLARE(opOptions, ApiOperation)

    //!
    //! \brief opGet
    //!
    Q_ANNOTATION_DECLARE(opGet, ApiOperation)

    //!
    //! \brief opPost
    //!
    Q_ANNOTATION_DECLARE(opPost, ApiOperation)

    //!
    //! \brief opPut
    //!
    Q_ANNOTATION_DECLARE(opPut, ApiOperation)

    //!
    //! \brief opDelete
    //!
    Q_ANNOTATION_DECLARE(opDelete, ApiOperation)

    //!
    //! \brief opRules
    //!
    Q_ANNOTATION_DECLARE_FUNC(opRules, Security)

    //!
    //! \brief rqExcludePath
    //!
    Q_ANNOTATION_DECLARE(rqExcludePath, Information)

    //!
    //! \brief rqSecurityIgnore
    //!
    Q_ANNOTATION_DECLARE(rqSecurityIgnore, Security)

    //!
    //! \brief apiRedirect
    //!
    Q_ANNOTATION_DECLARE(rqRedirect, Operation)
private:

};

}
