#include <QString>
#include <QtTest>

#include "../matrix34.h"
#include "../quaternion.h"

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

  Matrix3f total = rotX * rotY * rotZ;

  QVERIFY2(fabs(((total * total.inverse()) - Matrix3f::kIdentity).det()) < 0.00000001f, "Complex");

  Vector3f y_up = Vector3f(0.0f, 1.0f, 0.0f);
  Vector3f x_right = Vector3f(1.0f, 0.0f, 0.0f);

  Vector3f imageY = rotX * y_up;
  QVERIFY2(fabs(imageY.dot(y_up) - cosf(0.1f)) < 0.00001, "Rotation");

  Matrix3f rot_axis = Matrix3f::FromAxisAngle(y_up, 0.5);
  Vector3f imageX = rot_axis * y_up;
  QVERIFY2(fabs(imageX.dot(y_up) - y_up.lengthSquared()) < 0.00001, "Axis Angle");

  imageX = rot_axis * x_right;
  QVERIFY2(fabs(imageX.dot(x_right) - cosf(0.5)) < 0.00001, "Axis Angle");

  //Matrix3f identity = Matrix3f::kIdentity;
  QVERIFY2(true, "Failure");
}

QTEST_APPLESS_MAIN(Physics_testTest)

#include "tst_physics_testtest.moc"
