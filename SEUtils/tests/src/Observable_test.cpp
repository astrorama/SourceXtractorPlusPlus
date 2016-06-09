/**
 * @file tests/src/Observable_test.cpp
 * @date 05/20/16
 * @author mschefer
 */

#include <boost/test/unit_test.hpp>
#include "ElementsKernel/EnableGMock.h"
using namespace testing;


#include "SEUtils/Observable.h"

using namespace SEUtils;

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
  test_observable_a->ObservableA::addObserver(mock_observer_a);

  EXPECT_CALL(*mock_observer_a, handleMessage(Matcher<const TestMessageA&>(_)))
      .Times(1);

  test_observable_a->ObservableA::notifyObservers(TestMessageA());
}

BOOST_FIXTURE_TEST_CASE( removeObserver_test, ObservableFixture ) {
  test_observable_a->ObservableA::addObserver(mock_observer_a);

  test_observable_a->ObservableA::removeObserver(mock_observer_a);

  EXPECT_CALL(*mock_observer_a, handleMessage(Matcher<const TestMessageA&>(_)))
      .Times(0);

  test_observable_a->ObservableA::notifyObservers(TestMessageA());
}

BOOST_FIXTURE_TEST_CASE( multiple_observers_test, ObservableFixture ) {
  test_observable_a->ObservableA::addObserver(mock_observer_a);
  test_observable_a->ObservableA::addObserver(mock_observer_b);

  EXPECT_CALL(*mock_observer_a, handleMessage(Matcher<const TestMessageA&>(_)))
      .Times(1);
  EXPECT_CALL(*mock_observer_b, handleMessage(Matcher<const TestMessageA&>(_)))
      .Times(1);

  test_observable_a->ObservableA::notifyObservers(TestMessageA());
}

BOOST_FIXTURE_TEST_CASE( multiple_observables_test, ObservableFixture ) {
  test_observable_a->ObservableA::addObserver(mock_observer_a);
  test_observable_a->ObservableA::addObserver(mock_observer_b);
  test_observable_b->ObservableA::addObserver(mock_observer_a);

  EXPECT_CALL(*mock_observer_a, handleMessage(Matcher<const TestMessageA&>(_)))
      .Times(2);
  EXPECT_CALL(*mock_observer_b, handleMessage(Matcher<const TestMessageA&>(_)))
      .Times(1);

  test_observable_a->ObservableA::notifyObservers(TestMessageA());
  test_observable_b->ObservableA::notifyObservers(TestMessageA());
}

BOOST_FIXTURE_TEST_CASE( multiple_types_test, ObservableFixture ) {
  test_observable_a->ObservableA::addObserver(mock_observer_a);
  test_observable_a->ObservableB::addObserver(mock_observer_b);

  EXPECT_CALL(*mock_observer_a, handleMessage(Matcher<const TestMessageA&>(_)))
      .Times(1);
  EXPECT_CALL(*mock_observer_b, handleMessage(Matcher<const TestMessageB&>(_)))
      .Times(1);

  test_observable_a->ObservableA::notifyObservers(TestMessageA());
  test_observable_a->ObservableB::notifyObservers(TestMessageB());
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


