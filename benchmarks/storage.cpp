#include <variant>
#include <array>
#include <random>
#include <benchmark/benchmark.h>
#include <any>

using namespace std;

struct One {
  auto get () const { return 1; }
 };
struct Two {
  auto get() const { return 2; }
 };
struct Three { 
  auto get() const { return 3; }
};
struct Four {
  auto get() const { return 4; }
 };

template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...) -> overload<Ts...>;


std::random_device dev;
std::mt19937 rng(dev());
std::uniform_int_distribution<std::mt19937::result_type> random_pick(0,3); // distribution in range [1, 6]

template <std::size_t N>
std::array<int, N> get_random_array() {
  std::array<int, N> item;
  for (int i = 0 ; i < N; i++)
    item[i] = random_pick(rng);
  return item;
}


static void EmptyRandom(benchmark::State& state) {

  int index = 0;

  auto ran_arr = get_random_array<50>();
  int r = 0;

  auto pick_randomly = [&] () {
    index = (ran_arr[r % ran_arr.size()]);
    r++;
  };

  pick_randomly();


  for (auto _ : state) {

    pick_randomly();
    benchmark::DoNotOptimize(r);
  }

}
// Register the function as a benchmark
BENCHMARK(EmptyRandom);

static void TradeSpaceForPerformance(benchmark::State& state) {

  One one;
  Two two;
  Three three;
  Four four;
  int index = 0;

  auto ran_arr = get_random_array<50>();
  int r = 0;

  auto pick_randomly = [&] () {
    index = (ran_arr[r % ran_arr.size()]);
    r++;
  };

  pick_randomly();


  for (auto _ : state) {

    int res;
    switch (index) {
      case 0:
        res = one.get();
        break;
      case 1:
        res = two.get();
        break;
      case 2:
        res = three.get();
        break;
      case 3:
        res = four.get();
        break;
    }
    
    benchmark::DoNotOptimize(index);
    benchmark::DoNotOptimize(res);

    pick_randomly();
  }

}
// Register the function as a benchmark
BENCHMARK(TradeSpaceForPerformance);

static void Any(benchmark::State& state) {

  std::any value;
  int index = 0;

  auto ran_arr = get_random_array<50>();
  int r = 0;

  auto pick_randomly = [&] () {
    index = (ran_arr[r % ran_arr.size()]);
    r++;

    switch (index)
    {
      case 0:
        value = One();
        break;
      case 1:
        value = Two();
        break;
      case 2:
        value = Three();
        break;
      case 3:
        value = Four();
        break;
    }
  };

  pick_randomly();


  for (auto _ : state) {

    int res;
    switch (index) {
      case 0:
        res = std::any_cast<One>(value).get();
        break;
      case 1:
        res = std::any_cast<Two>(value).get();
        break;
      case 2:
        res = std::any_cast<Three>(value).get();
        break;
      case 3:
        res = std::any_cast<Four>(value).get();
        break;
    }
    
    benchmark::DoNotOptimize(index);
    benchmark::DoNotOptimize(res);

    pick_randomly();
  }

}
// Register the function as a benchmark
BENCHMARK(Any);

#define CMAX(a, b) (((a) > (b)) ? (a) : (b))

static void Cast(benchmark::State& state) {
  char data[CMAX(sizeof(One), CMAX(sizeof(Two), CMAX(sizeof(Three), sizeof(Four))))];
  int type;
  void *ptr = nullptr;

  auto ran_arr = get_random_array<50>();
  int r = 0;

  auto pick_randomly = [&] () {
    if (ptr != nullptr) {
      switch (type) {
        case 0:
          static_cast<One*>(ptr)->~One();
          break;
        case 1:
          static_cast<Two*>(ptr)->~Two();
          break;
        case 2:
          static_cast<Three*>(ptr)->~Three();
          break;
        case 3:
          static_cast<Four*>(ptr)->~Four();
          break;
      }
    }

    type = (ran_arr[r % ran_arr.size()]);
    r++;

    switch (type) {
      case 0:
        ptr = static_cast<void*>(new (&data) One);
        break;
      case 1:
        ptr = static_cast<void*>(new (&data) Two);
        break;
      case 2:
        ptr = static_cast<void*>(new (&data) Three);
        break;
      case 3:
        ptr = static_cast<void*>(new (&data) Four);
        break;
    }
  };

  pick_randomly();

  for (auto _ : state) {

    int res;
    switch (type) {
      case 0:
        res = static_cast<One*>(ptr)->get();
        break;
      case 1:
        res = static_cast<Two*>(ptr)->get();
        break;
      case 2:
        res = static_cast<Three*>(ptr)->get();
        break;
      case 3:
        res = static_cast<Four*>(ptr)->get();
        break;
    }

    benchmark::DoNotOptimize(ptr);
    benchmark::DoNotOptimize(res);

    pick_randomly();
  }
}
BENCHMARK(Cast);

static void Virtual(benchmark::State& state) {

  struct Base {
    virtual int get() const noexcept = 0;
    virtual ~Base() {}
  };

  struct A final: public Base {
    int get()  const noexcept override { return 1; }
  };

  struct B final : public Base {
    int get() const noexcept override { return 2; }
  };

  struct C final : public Base {
    int get() const noexcept override { return 3; }
  };

  struct D final : public Base {
    int get() const noexcept override { return 4; }
  };

  Base* package = nullptr;
  auto ran_arr = get_random_array<50>();
  int r = 0;

  auto pick_randomly = [&] () {
    delete package;
    switch(ran_arr[r % ran_arr.size()]) {
      case 0:
        package = new A;
        break;
      case 1:
        package = new B;
        break;
      case 2:
        package = new C;
        break;
      case 3:
        package = new D;
        break;
    }
    r++;
  };

  pick_randomly();

  for (auto _ : state) {

    int res = package->get();

    benchmark::DoNotOptimize(package);
    benchmark::DoNotOptimize(res);

    pick_randomly();
  }

  delete package;
}
//BENCHMARK(Virtual);


static void Index(benchmark::State& state) {


  std::variant<One, Two, Three, Four> package { One() };

  auto ran_arr = get_random_array<50>();
  int r = 0;

  auto pick_randomly = [&] () {
    switch(ran_arr[r % ran_arr.size()]) {
      case 0:
        package = One();
        break;
      case 1:
        package = Two();
        break;
      case 2:
        package = Three{};
        break;
      case 3:
        package = Four{};
        break;
    }
    r++;
  };


  pick_randomly();

  for (auto _ : state) {

    int res;
    switch (package.index()) {
      case 0: 
        res = std::get<One>(package).get();
        break;
      case 1:
        res = std::get<Two>(package).get();
        break;
      case 2:
        res = std::get<Three>(package).get();
        break;
      case 3:
        res = std::get<Four>(package).get();
        break;
    }

    benchmark::DoNotOptimize(package);
    benchmark::DoNotOptimize(res);

    pick_randomly();
  }
}
BENCHMARK(Index);



static void GetIf(benchmark::State& state) {
  std::variant<One, Two, Three, Four> package { 
        One() };

  auto ran_arr = get_random_array<50>();
  int r = 0;

  auto pick_randomly = [&] () {
    switch(ran_arr[r % ran_arr.size()]) {
      case 0:
        package = One();
        break;
      case 1:
        package = Two();
        break;
      case 2:
        package = Three{};
        break;
      case 3:
        package = Four{};
        break;
    }
    r++;
  };

  pick_randomly();

  for (auto _ : state) {

    int res;
    if (auto item = std::get_if<One>(&package)) {
      res = item->get();
    } else if (auto item = std::get_if<Two>(&package)) {
      res = item->get();
    } else if (auto item = std::get_if<Three>(&package)) {
      res = item->get();
    } else if (auto item = std::get_if<Four>(&package)) {
      res = item->get();
    }

    benchmark::DoNotOptimize(package);
    benchmark::DoNotOptimize(res);

    pick_randomly();
  }
}
BENCHMARK(GetIf);

static void HoldsAlternative(benchmark::State& state) {
  std::variant<One, Two, Three, Four> package { 
        One() };

  auto ran_arr = get_random_array<50>();
  int r = 0;

  auto pick_randomly = [&] () {
    switch(ran_arr[r % ran_arr.size()]) {
      case 0:
        package = One();
        break;
      case 1:
        package = Two();
        break;
      case 2:
        package = Three{};
        break;
      case 3:
        package = Four{};
        break;
    }
    r++;
  };


  pick_randomly();

  for (auto _ : state) {

    int res;
    if (std::holds_alternative<One>(package)) {
      res = std::get<One>(package).get();
    } else if (std::holds_alternative<Two>(package)) {
      res = std::get<Two>(package).get();
    } else if (std::holds_alternative<Three>(package)) {
      res = std::get<Three>(package).get();
    } else if (std::holds_alternative<Four>(package)) {
      res = std::get<Four>(package).get();
    }

    benchmark::DoNotOptimize(package);
    benchmark::DoNotOptimize(res);

    pick_randomly();
  }
}
BENCHMARK(HoldsAlternative);


static void ConstexprVisitor(benchmark::State& state) {
  std::variant<One, Two, Three, Four> package { 
        One() };

  auto ran_arr = get_random_array<50>();
  int r = 0;

  auto pick_randomly = [&] () {
    switch(ran_arr[r % ran_arr.size()]) {
      case 0:
        package = One();
        break;
      case 1:
        package = Two();
        break;
      case 2:
        package = Three{};
        break;
      case 3:
        package = Four{};
        break;
    }
    r++;
  };

  pick_randomly();

  for (auto _ : state) {

    auto res = std::visit([] (auto const& ref) {
        using type = std::decay_t<decltype(ref)>;
        if constexpr (std::is_same<type, One>::value) {
            return ref.get();
        } else if constexpr (std::is_same<type, Two>::value) {
            return ref.get();
        } else if constexpr (std::is_same<type, Three>::value)  {
          return ref.get();
        } else if constexpr (std::is_same<type, Four>::value) {
            return ref.get();
        } else {
          return 0;
        }
    }, package);

    benchmark::DoNotOptimize(package);
    benchmark::DoNotOptimize(res);

    pick_randomly();
  }
}
BENCHMARK(ConstexprVisitor);

static void StructVisitor(benchmark::State& state) {

  

  struct VisitPackage
  {
      auto operator()(One const &r) { return r.get(); }
      auto operator()(Two const &r) { return r.get(); }
      auto operator()(Three const &r) { return r.get(); }
      auto operator()(Four const &r) { return r.get(); }
  };

  std::variant<One, Two, Three, Four> package { 
        One() };

  auto ran_arr = get_random_array<50>();
  int r = 0;

  auto pick_randomly = [&] () {
    switch(ran_arr[r % ran_arr.size()]) {
      case 0:
        package = One();
        break;
      case 1:
        package = Two();
        break;
      case 2:
        package = Three{};
        break;
      case 3:
        package = Four{};
        break;
    }
    r++;
  };

  pick_randomly();

  for (auto _ : state) {

    auto res = std::visit(VisitPackage(), package);

    benchmark::DoNotOptimize(package);
    benchmark::DoNotOptimize(res);

    pick_randomly();
  }
}
BENCHMARK(StructVisitor);


static void Overload(benchmark::State& state) {

  std::variant<One, Two, Three, Four> package { 
        One() };

  auto ran_arr = get_random_array<50>();
  int r = 0;

  auto pick_randomly = [&] () {
    switch(ran_arr[r % ran_arr.size()]) {
      case 0:
        package = One();
        break;
      case 1:
        package = Two();
        break;
      case 2:
        package = Three{};
        break;
      case 3:
        package = Four{};
        break;
    }
    r++;
  };

  pick_randomly();

  for (auto _ : state) {

    auto res = std::visit(overload {
      [] (One const& r) { return r.get(); },
      [] (Two const& r) { return r.get(); },
      [] (Three const& r) { return r.get(); },
      [] (Four const& r) { return r.get(); }
    }, package);

  
    benchmark::DoNotOptimize(package);
    benchmark::DoNotOptimize(res);

    pick_randomly();
  }

}
BENCHMARK(Overload);

int main(int argc, char** argv)
{
    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();

    return 0;
}
