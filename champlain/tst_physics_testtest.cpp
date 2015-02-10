#include <QString>
#include <QtTest>

#include "../matrix34.h"

class Physics_testTest : public QObject
{
  Q_OBJECT

public:
  Physics_testTest();

private Q_SLOTS:
  void testCase1();
};

Physics_testTest::Physics_testTest()
{
}

#include <iostream>

void Physics_testTest::testCase1()
{
  Matrix3f matrix(Matrix3f::kZero);
  Matrix3f identity(Matrix3f::kIdentity);

  Matrix3f sum(identity + identity);
  Matrix3f sum2 = identity * 2.0f;

  QVERIFY2(sum2 - sum == Matrix3f::kZero, "Sum" );
  QVERIFY2(sum2.det() == 8.0f, "Det");

  Matrix3f inverse = sum2.inverse();
  QVERIFY2(inverse.det() - (1.0f/8.0f) < 0.01f, "Inverse");

  QVERIFY2(inverse*sum2 == Matrix3f::kIdentity, "Inverse2");

  Matrix3f rotX = Matrix3f::RotationX(0.1);
  Matrix3f rotY = Matrix3f::RotationY(0.5);
  Matrix3f rotZ = Matrix3f::RotationZ(2.0);

  Matrix3f total = rotX; // * rotY * rotZ;

  std::cout << (bool)(((total * total.inverse()) - Matrix3f::kIdentity) == Matrix3f::kIdentity);
  QVERIFY2(fabs(((total * total.inverse()) - Matrix3f::kIdentity).det()) < 0.00000001f, "Complex");

  //Matrix3f identity = Matrix3f::kIdentity;
  QVERIFY2(true, "Failure");
}

QTEST_APPLESS_MAIN(Physics_testTest)

#include "tst_physics_testtest.moc"
