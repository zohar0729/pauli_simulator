# プログラム名とオブジェクト名
PROG := pauli_simulator
SRCS := main.cpp pauli_simulator.cpp decoder.cpp random.cpp
OBJS := $(SRCS:.cpp=.o)
DEPS := $(SRCS:.cpp=.d)

# 定義済みマクロの再定義
CC = gcc-7
CXX = g++-7
CPPFLAGS = -O2

#プライマリターゲット
all: $(PROG)

# ヘッダーファイルの変更を検知
include $(DEPS)

$(PROG) : $(OBJS)
	$(CXX) -o $(PROG) $^

# パターンルール
%.d: %.cpp
	@set -e; rm -f $@; \
	$(CXX) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

.PHONY: clean
clean:
	$(RM) $(PROG) $(OBJS) $(DEPS)