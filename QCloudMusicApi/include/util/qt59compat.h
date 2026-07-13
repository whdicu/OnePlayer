#ifndef QT59COMPAT_H
#define QT59COMPAT_H

#include <QtGlobal>

#if QT_VERSION < QT_VERSION_CHECK(5, 10, 0)
// QRandomGenerator only available since Qt 5.10
// Provide qrand()-based fallback
#include <QtGlobal>
#include <cstdlib>

inline int qt_compat_bounded(int max) {
    if (max <= 0) return 0;
    return qrand() % max;
}

inline int qt_compat_bounded(int min, int max) {
    if (max <= min) return min;
    return min + (qrand() % (max - min));
}

inline double qt_compat_bounded_double(double max) {
    return (qrand() / static_cast<double>(RAND_MAX)) * max;
}

inline quint32 qt_compat_generate32() {
    return (static_cast<quint32>(qrand()) & 0xFFFF)
         | (static_cast<quint32>(qrand()) << 16);
}

#define QRANDOM_BOUNDED(max)         qt_compat_bounded(max)
#define QRANDOM_BOUNDED_RANGE(a,b)   qt_compat_bounded(a, b)
#define QRANDOM_BOUNDED_DOUBLE(max)  qt_compat_bounded_double(max)
#define QRANDOM_GENERATE32()         qt_compat_generate32()

#else
// Qt 5.10+ - use native QRandomGenerator
#include <QRandomGenerator>

#define QRANDOM_BOUNDED(max)         QRandomGenerator::global()->bounded(max)
#define QRANDOM_BOUNDED_RANGE(a,b)   QRandomGenerator::global()->bounded(a, b)
#define QRANDOM_BOUNDED_DOUBLE(max)  QRandomGenerator::global()->bounded(max)
#define QRANDOM_GENERATE32()         QRandomGenerator::global()->generate()
#endif

#endif // QT59COMPAT_H
