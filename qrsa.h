#ifndef QRSA_H
#define QRSA_H

#include "qchat_pch.h"
#include "gmp/include/gmp.h"

enum rsaKeyType {
    rkPublic,
    rkPublicAndPrivate
};

class QRsaKey
{
    friend class QRsa;
public:
    QRsaKey();
    QRsaKey(const QRsaKey& key);
    QRsaKey(const QByteArray& base64key);
    QRsaKey& operator= (const QRsaKey& key);
    ~QRsaKey();

    QByteArray privateKey();
    QByteArray publicKey();
    QByteArray module();

    static QRsaKey& local();
private:
    bool valid;
    rsaKeyType keyType;
    quint16 keyBits;
    mpz_t n;
    mpz_t e;
    mpz_t d;

    void generateKey();
};

class QRsa
{

private:
    QRsa() {;}
    QRsa(const QRsa& ) {;}
    void operator= (const QRsa& ) {;}

public:
    static QByteArray encrypt(const QString& msg, const QRsaKey& key);
    static QString decrypt(const QByteArray& msg, const QRsaKey& key);
};

#endif // QRSA_H
