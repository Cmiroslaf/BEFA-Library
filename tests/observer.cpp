//
// Created by miro on 16/10/16.
//


#include <gtest/gtest.h>
#include <befa/utils/observer.hpp>

#include "fixtures.hpp"

namespace {

TEST(ObservableTest, RXTest) {
  Subject<int> subj;
  Observable<int> o$(subj);

  o$.subscribe([](const int &val) {
    ASSERT_EQ(10, val);
  });

  subj.update(10);
}

TEST(ObservableTest, SubjectAsObservable) {
  Subject<int> subj;
  Observable<int> o$ = subj.toObservable();

  o$.subscribe([](const int &val) {
    ASSERT_EQ(10, val);
  });

  subj.update(10);
}

typedef Subject<std::string> rx_subject_t;
typedef rx_subject_t::subscription_type subscription_t;
typedef rx_subject_t::observable_type observable_t;

TEST(ObservableTest, SubscriptionTest) {
  rx_subject_t subj;
  observable_t o$(subj);

  std::vector<subscription_t> subscriptions;

  for (int i = 0; i < 200; ++i) {
    subscriptions.push_back(o$.subscribe([](const std::string &val) {
      ASSERT_EQ("String", val);
    }));
  }

  subj.update("String");

  for (auto &s : subscriptions)
    s.unsubscribe();

  subj.update("WrongString");
}

TEST(ObservableTest, MoveTest) {
  auto create_subject = []() {
    rx_subject_t subj;
    subj.update("Trololo");
    return subj;
  };

  rx_subject_t subj(create_subject());
  observable_t o$(subj);

  std::vector<subscription_t> subscriptions;
  int count = 0;
  for (int i = 0; i < 200; ++i) {
    subscriptions.push_back(o$.subscribe([&](const std::string &val) {
      ASSERT_EQ("String", val);
      ++count;
    }));
  }

  subj.update("String");

  ASSERT_EQ(count, 200);

  for (auto &s : subscriptions)
    s.unsubscribe();

  subj.update("WrongString");
}

TEST(ObservableTest, MapTest) {
  rx_subject_t subj;
  observable_t so$(subj);

  auto subscription = so$.map<size_t>([](const std::string &s) {
    return s.size();
  }).subscribe([](size_t size) {
    ASSERT_EQ(size, 5);
  });

  subj.update("12345");
}

TEST(ObservableTest, ConditionalTest) {
  rx_subject_t subj;
  observable_t so$(subj);

  auto subscription = so$.conditional([](const std::string &s) {
    return s == "a";
  }).subscribe([](const std::string &s) {
    ASSERT_EQ(s, "a");
  });

  subj.update("12345");
  subj.update("a");
  subj.update("aa");
}

struct RegexSearch {
  /**
   * Thrown, if this cannot provide further data
   */
  struct NextIterationError : public std::exception {
    const char *what() const noexcept override {
      return "cannot continue to iterate";
    }
  };

  RegexSearch(std::string input, std::string pattern) :
      str(input), input(str), pattern(pattern) {}

  // ~~~~~ Move & Copy semantics ~~~~~
  RegexSearch(RegexSearch &&rhs) :
      str(std::move(rhs.str)), input(str), pattern(std::move(rhs.pattern)) {}
  RegexSearch(const RegexSearch &rhs) :
      str(rhs.str), input(str), pattern(rhs.pattern) {}
  RegexSearch &operator=(RegexSearch &&rhs) {
    str = std::move(rhs.str);
    input = pcrecpp::StringPiece(str);
    pattern = std::move(rhs.pattern);
    return *this;
  }
  RegexSearch &operator=(const RegexSearch &rhs) {
    str = rhs.str;
    input = pcrecpp::StringPiece(str);
    pattern = rhs.pattern;
    return *this;
  }
  // ~~~~~ Move & Copy semantics ~~~~~

  std::string operator()() {
    std::string output;
    if (pattern.FindAndConsume(&input, &output))
      return output;
    throw NextIterationError();
  }

 private:
  std::string str;
  pcrecpp::StringPiece input;
  pcrecpp::RE pattern;
};

TEST(ObservableTest, NextTest) {
  rx_subject_t subj(RegexSearch("stringstriingstriiing", "(stri*ng)"));
  observable_t so$(subj);

  std::string return_value = "string";
  auto subscription = so$.subscribe([&return_value](const std::string &s) {
    ASSERT_EQ(s, return_value);
  });

  subj.next();
  return_value = "striing";
  subj.next();
  return_value = "striiing";
  subj.next();

  ASSERT_THROW(subj.next(), RegexSearch::NextIterationError);
}

TEST(ObservableTest, CopyFromScopeTest) {
  std::string return_value;
  int i = 0;

  auto create$ = [&] {
    rx_subject_t subj(RegexSearch("stringstriingstriiing", "(stri*ng)"));
    auto subscription = subj.subscribe([&](const std::string &s) {
      ++i;
      ASSERT_EQ(s, return_value);
    });
    return subj;
  };

  auto subj = create$();

  ASSERT_EQ(i, 0);
  return_value = "string";
  subj.next();
  ASSERT_EQ(i, 1);
  return_value = "striing";
  subj.next();
  ASSERT_EQ(i, 2);
  return_value = "striiing";
  subj.next();
  ASSERT_EQ(i, 3);

  ASSERT_THROW(subj.next(), RegexSearch::NextIterationError);
}

TEST(ObservableTest, OperatorTest) {
  int num = 0;
  Subject<int> subj([&num] { return ++num; });

  int res = 1;
  subj.asObservable()
      >> [&res](const int &n) { ASSERT_EQ(n, res); }
      >> [&res](const int &n) { ASSERT_EQ(n, res); }
      >> [&res](const int &n) { ASSERT_EQ(n, res); };

  for (int i = 0; i < 10; ++i, ++res)
    subj.next();
}

template<typename T>
struct ConditionalObservable : public Observable<T> {
  template<typename ConditionalT>
  ConditionalObservable(ConditionalT &&cond, Observable<T> &o$) {
    o$.subscribe([&](const T &val) {
      if (cond(val)) this->notify(val);
    });
  }
};

TEST(ObservableTest, ConditionalObservableTest) {
  int num = 0;
  Subject<int> subj([&num] { return ++num; });

  ConditionalObservable<int>(
      [](const int &n) { return n == 2; },
      subj.asObservable()
  ) >> [](const int &n) { ASSERT_EQ(n, 2); };

  for (int i = 0; i < 10; ++i)
      subj.next();
}

}  // namespace

