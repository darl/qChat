#ifndef QRSA_H
#define QRSA_H

#include "qchat_pch.h"
#include "gmp.h"

enum rsaKeyType {
    rkPublic,
    rkPublicAndPrivate
};

class qRsaKey
{
    friend class qRsa;
public:
    qRsaKey();
    qRsaKey(const qRsaKey& key);
    qRsaKey(const QByteArray& base64key);
    qRsaKey& operator= (const qRsaKey& key);
    ~qRsaKey();

    QByteArray privateKey();
    QByteArray publicKey();
    QByteArray module();

    static qRsaKey& local();
private:
    bool valid;
    rsaKeyType keyType;
    quint16 keyBits;
    mpz_t n;
    mpz_t e;
    mpz_t d;

    void generateKey();
};

class qRsa
{
public:
    static QByteArray encrypt(const QString& msg, const qRsaKey& key);
    static QString decrypt(const QByteArray& msg, const qRsaKey& key);

private:
    qRsa();
    qRsa(const qRsa&);
    void operator= (const qRsa&);
};

#endif // QRSA_H
