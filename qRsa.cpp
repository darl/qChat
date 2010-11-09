#include "qRsa.h"
#include "mt64.h"

#define KEYBITS 1024
#define PQBITS (KEYBITS>>1)

void check_for_prime(mpz_ptr p)
{
    int forprimep = mpz_probab_prime_p (p, 50);
    while (forprimep == 0)
    {
        mpz_add_ui (p, p, 1);
        forprimep = mpz_probab_prime_p (p, 50);
    };
}

QRsaKey& QRsaKey::local()
{
    static QRsaKey key;
    if(!key.valid) key.generateKey();
    return key;
}

QRsaKey::QRsaKey()
{
    valid = false;
}

QRsaKey::QRsaKey(const QRsaKey& key) : keyBits(key.keyBits)
{
    keyType = key.keyType;
    valid = key.valid;

    mpz_init(n);
    mpz_set(n, key.n);

    mpz_init(e);
    mpz_set(e, key.e);

    if(keyType == rkPublicAndPrivate)
    {
        mpz_init(d);
        mpz_set(d, key.d);
    }
}

QRsaKey::QRsaKey(const QByteArray& base64key)
{
    keyType = rkPublic;
    valid = false;
    QList<QByteArray> keys(base64key.split(':'));
    if(keys.count() != 2)
        qCritical() << "invalid public key";
    QByteArray a = QByteArray::fromBase64(keys[0]);
    QByteArray b = QByteArray::fromBase64(keys[1]);

    mpz_init(e);
    mpz_import(e, a.size(), 1, 1, 0, 0, a.constData());
    mpz_init(n);
    mpz_import(n, b.size(), 1, 1, 0, 0, b.constData());
    valid = true;
}

QRsaKey& QRsaKey::operator=(const QRsaKey& key)
{
    if(valid)
    {
        mpz_clear(n);
        mpz_clear(e);
        if(keyType == rkPublicAndPrivate)
            mpz_clear(d);
    }

    keyBits = key.keyBits;
    keyType = key.keyType;

    valid = key.valid;
    if(valid)
    {
        mpz_init(n);
        mpz_set(n, key.n);

        mpz_init(e);
        mpz_set(e, key.e);

        if(keyType == rkPublicAndPrivate)
        {
            mpz_init(d);
            mpz_set(d, key.d);
        }
    }

    return *this;
}

QRsaKey::~QRsaKey()
{
    if(!valid) return;
    mpz_clear(n);
    mpz_clear(e);
    if(keyType == rkPublicAndPrivate)
        mpz_clear(d);
}

QByteArray QRsaKey::privateKey()
{
    if(!valid) return QByteArray("invalid key");
    if(keyType == rkPublic) return QByteArray("no private key");
    size_t rl = 0;
    char* r = (char*)mpz_export(NULL, &rl, 1, 1, 0, 0, d);
    return QByteArray(r, rl);
}

QByteArray QRsaKey::publicKey()
{
    if(!valid) return QByteArray("invalid key");
    size_t rl = 0;
    char* r = (char*)mpz_export(NULL, &rl, 1, 1, 0, 0, e);
    return QByteArray(r, rl);
}

QByteArray QRsaKey::module()
{
    if(!valid) return QByteArray("invalid key");
    size_t rl = 0;
    char* r = (char*)mpz_export(NULL, &rl, 1, 1, 0, 0, n);
    return QByteArray(r, rl);
}

void QRsaKey::generateKey()
{
    mpz_t p, q;

    // Инициализация рандома
    gmp_randstate_t state;
    gmp_randinit_default(state);
    gmp_randseed_ui(state, genrand64_int64());

    // 1. Генерируется случайные простые числа p и q
    mpz_init2(p,PQBITS);
    mpz_urandomb(p, state, PQBITS);

    check_for_prime(p);

    mpz_init2(q,PQBITS);
    mpz_urandomb(q, state, PQBITS);

    check_for_prime(q);

    // 2. Вычисляется их произведение n = pq
    mpz_init(n);
    mpz_mul(n,p,q);

    // 3. Вычисляется значение функция Эйлера от числа n
    mpz_t fi;
    mpz_init(fi);

    mpz_t p_m, q_m;
    mpz_init(p_m);
    mpz_init(q_m);
    mpz_sub_ui(p_m, p, 1);
    mpz_sub_ui(q_m, q, 1);

    mpz_mul(fi, p_m, q_m);


    // 4.Выбирается целое число e, взаимно простое со значением функции fi
    // часто выбирают один из простых чисел Ферма 17, 257, или 65537
    int possible_values[] = {
        17, 257, 65537
            };

    mpz_init(e);
    mpz_t gcd;
    mpz_init(gcd);
    int i = 0;
    for (; i < 3; ++i)
    {
        mpz_set_ui(e,possible_values[i]);
        mpz_gcd(gcd, e, fi);
        if(mpz_cmp_ui(gcd, 1) == 0)
            break;
    }

    if(i == 3)
        qCritical() << "no digit selected";

    // 5. Вычисляется число d мультипликативное обратное к числу e по модулю fi
    mpz_init(d);

    mpz_invert(d, e, fi);

    keyBits = KEYBITS;

    mpz_clear(p);
    mpz_clear(q);
    mpz_clear(fi);
    mpz_clear(p_m);
    mpz_clear(q_m);
    mpz_clear(gcd);

    keyType = rkPublicAndPrivate;
    valid = true;
}

QByteArray QRsa::encrypt(const QString& msg, const QRsaKey& key)
{
    if(!key.valid) return QByteArray("invalid key");
    mpz_t M;
    mpz_init(M);

    mpz_import(M, msg.size(), 1, 1, 0, 0, msg.toAscii().constData());

    mpz_t rez;
    mpz_init(rez);

    mpz_powm(rez, M, key.e, key.n);

    size_t rl = 0;
    char* r = (char*)mpz_export(NULL, &rl, 1, 1, 0, 0, rez);

    mpz_clear(M);
    mpz_clear(rez);

    QByteArray a(r, rl);
    free(r);
    return a;
}

QString QRsa::decrypt(const QByteArray& msg, const QRsaKey& key)
{
    if(!key.valid) return QByteArray("invalid key");
    if(key.keyType == rkPublic) return QString("no private key");
    mpz_t M;
    mpz_init(M);
    mpz_import(M, msg.size(), 1, 1, 0, 0, msg.constData());

    mpz_t rez;
    mpz_init(rez);

    mpz_powm(rez, M, key.d, key.n);
    size_t rl = 0;
    char* r = (char*)mpz_export(NULL, &rl, 1, 1, 0, 0, rez);

    mpz_clear(M);
    mpz_clear(rez);

    QString a = QString::fromAscii(r, rl);
    free(r);
    return a;
}
