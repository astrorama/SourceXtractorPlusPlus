/** Copyright © 2019 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
/**
 * @file tests/src/Observable_test.cpp
 * @date 05/20/16
 * @author mschefer
 */

#include <boost/test/unit_test.hpp>
#include "ElementsKernel/EnableGMock.h"
using namespace testing;


#include "SEUtils/Observable.h"

using namespace SourceXtractor;

class TestMessageA {

};

class TestMessageB {

};

using ObserverA = Observer<TestMessageA>;
using ObservableA = Observable<TestMessageA>;
using ObserverB = Observer<TestMessageB>;
using ObservableB = Observable<TestMessageB>;

class MockObserver : public ObserverA, public ObserverB {
public:
  MOCK_METHOD1(handleMessage, void (const TestMessageA&));
  MOCK_METHOD1(handleMessage, void (const TestMessageB&));
};

class TestObservable : public ObservableA, public ObservableB {
public:
    using ObservableA::notifyObservers;
  using ObservableB::notifyObservers;
};

struct ObservableFixture {
  std::shared_ptr<MockObserver> mock_observer_a, mock_observer_b;
  std::shared_ptr<TestObservable> test_observable_a, test_observable_b;

  ObservableFixture()
    : mock_observer_a(new MockObserver), mock_observer_b(new MockObserver),
      test_observable_a(new TestObservable), test_observable_b(new TestObservable) {}
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (Observable_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( simple_notification_test, ObservableFixture ) {
  // We add mock_observer_a to test_observable_a
  test_observable_a->ObservableA::addObserver(mock_observer_a);

  // we expect that the Observer is going to be notified
  EXPECT_CALL(*mock_observer_a, handleMessage(Matcher<const TestMessageA&>(_)))
      .Times(1);

  // Request that the Observers should be notified
  test_observable_a->notifyObservers(TestMessageA());
}

BOOST_FIXTURE_TEST_CASE( removeObserver_test, ObservableFixture ) {
  // We add mock_observer_a to test_observable_a
  test_observable_a->ObservableA::addObserver(mock_observer_a);

  // We then remove it
  test_observable_a->ObservableA::removeObserver(mock_observer_a);

  // It should not be notified since it was removed
  EXPECT_CALL(*mock_observer_a, handleMessage(Matcher<const TestMessageA&>(_)))
      .Times(0);

  // Request that the Observers should be notified
  test_observable_a->notifyObservers(TestMessageA());
}

BOOST_FIXTURE_TEST_CASE( multiple_observers_test, ObservableFixture ) {
  // We add both mock_observer_a mock_observer_b and to test_observable_a
  test_observable_a->ObservableA::addObserver(mock_observer_a);
  test_observable_a->ObservableA::addObserver(mock_observer_b);

  // We expect each one will be notified once
  EXPECT_CALL(*mock_observer_a, handleMessage(Matcher<const TestMessageA&>(_)))
      .Times(1);
  EXPECT_CALL(*mock_observer_b, handleMessage(Matcher<const TestMessageA&>(_)))
      .Times(1);

  // Request that the Observers should be notified
  test_observable_a->notifyObservers(TestMessageA());
}

BOOST_FIXTURE_TEST_CASE( multiple_observables_test, ObservableFixture ) {
  // We add both mock_observer_a mock_observer_b and to test_observable_a
  test_observable_a->ObservableA::addObserver(mock_observer_a);
  test_observable_a->ObservableA::addObserver(mock_observer_b);
  // We also add mock_observer_a to test_observable_b
  test_observable_b->ObservableA::addObserver(mock_observer_a);

  // We expect mock_observer_a to be notified twice and mock_observer_b once
  EXPECT_CALL(*mock_observer_a, handleMessage(Matcher<const TestMessageA&>(_)))
      .Times(2);
  EXPECT_CALL(*mock_observer_b, handleMessage(Matcher<const TestMessageA&>(_)))
      .Times(1);

  // Request from each Observable that the Observers should be notified
  test_observable_a->notifyObservers(TestMessageA());
  test_observable_b->notifyObservers(TestMessageA());
}

BOOST_FIXTURE_TEST_CASE( multiple_types_test, ObservableFixture ) {
  // We add mock_observer_a to test_observable_a to listen to TestMessageA
  test_observable_a->ObservableA::addObserver(mock_observer_a);
  // We add mock_observer_b to test_observable_a to listen to TestMessageB
  test_observable_a->ObservableB::addObserver(mock_observer_b);

  // We expect mock_observer_a to receive a TestMessageA notification and mock_observer_b a TestMessageB notification
  EXPECT_CALL(*mock_observer_a, handleMessage(Matcher<const TestMessageA&>(_)))
      .Times(1);
  EXPECT_CALL(*mock_observer_b, handleMessage(Matcher<const TestMessageB&>(_)))
      .Times(1);

  // Request from the  Observable a TestMessageA notification and a TestMessageB notification
  test_observable_a->notifyObservers(TestMessageA());
  test_observable_a->notifyObservers(TestMessageB());
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


