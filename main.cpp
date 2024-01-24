#include <QCoreApplication>
#include <QThread>
#include <QSemaphore>
#include <QDebug>

const int N = 5; // Número máximo de procesos en ejecución
int Ent = 0;

// Clase que representa un proceso A
class ProcesoA : public QThread
{
public:
    ProcesoA(int id, QSemaphore &semB, QSemaphore &semShared)
        : id(id), semB(semB), semShared(semShared) {}

protected:
    void run() override
    {
        qDebug() << "Proceso A"<<id << ": Solicitarlo";
        semShared.acquire(2); // Espera por al menos 2 procesos
        qDebug() << "Proceso A"<<id << ": Entrar";
        Ent++;
        qDebug() << "Valor de Ent:" << Ent;

        // Simular el uso del recurso compartido
        qDebug() << "Proceso A"<<id << ": Usarlo";
        QThread::msleep(2000); // Simulación de uso
        qDebug() << "Proceso A"<<id << ": Liberarlo";
        Ent--;

        semShared.release();
        qDebug() << "Valor de Ent:" << Ent;
    }

private:
    int id;
    QSemaphore &semB;
    QSemaphore &semShared;
};

// Clase que representa un proceso B
class ProcesoB : public QThread
{
public:
    ProcesoB(int id, QSemaphore &semShared)
        : id(id), semShared(semShared) {}

protected:
    void run() override
    {
        qDebug() << "Proceso B"<<id << ": Solicitarlo";
        semShared.acquire(); // Esperar a que el recurso compartido esté disponible
        qDebug() << "Proceso B"<<id << ": Entrar";
        Ent++;
        qDebug() << "Valor de Ent:" << Ent;

        // Simular el uso del recurso compartido
        qDebug() << "Proceso B"<<id << ": Usarlo";
        QThread::msleep(2000); // Simulación de uso
        qDebug() << "Proceso B"<<id << ": Liberarlo";
        Ent--;

        semShared.release(); // Liberar el recurso compartido
        qDebug() << "Valor de Ent:" << Ent;
    }

private:
    int id;
    QSemaphore &semShared;
};

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QSemaphore semShared(N+1); // Inicialmente, hay N permisos disponibles
    QSemaphore semB(0); // Inicialmente, no hay procesos B disponibles

    // Crear instancias de 10 procesos A y 10 procesos B
    QList<ProcesoA*> procesosA;
    QList<ProcesoB*> procesosB;

    for (int i = 0; i < 10; ++i) {
        procesosA.append(new ProcesoA(i + 1, semB, semShared));
        procesosB.append(new ProcesoB(i + 1, semShared));
    }

    // Iniciar los procesos
    for (int i = 0; i < 10; ++i) {
        procesosA[i]->start();
        procesosB[i]->start();
    }

    // Esperar a que los procesos terminen
    for (int i = 0; i < 10; ++i) {
        procesosA[i]->wait();
        procesosB[i]->wait();
    }

    // Liberar la memoria de las instancias
    qDeleteAll(procesosA);
    qDeleteAll(procesosB);

    return a.exec();
}
