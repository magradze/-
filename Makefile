#
# Makefile "მანუსკრიპტი" პროექტისთვის
# ავტორი: გიორგი მაღრაძე
# ვებგვერდი: https://magradze.dev
# GitHub: https://github.com/magradze
#

# კომპილატორი და მისი პარამეტრები
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -g -Iinclude

# დირექტორიები
SRC_DIR = src
RUNTIME_DIR = runtime
BUILD_DIR = build
EXAMPLES_DIR = examples

# საბოლოო შესრულებადი ფაილი
TARGET = $(BUILD_DIR)/manuscript

# ვპოულობთ ყველა .c ფაილს src და runtime დირექტორიებში
SRC_FILES = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(RUNTIME_DIR)/*.c)
# ვაქცევთ .c ფაილებს .o (object) ფაილებად build დირექტორიაში
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(wildcard $(SRC_DIR)/*.c))
OBJ_FILES += $(patsubst $(RUNTIME_DIR)/%.c,$(BUILD_DIR)/%.o,$(wildcard $(RUNTIME_DIR)/*.c))

# ნაგულისხმევი წესი: ააწყვეს პროექტი
all: $(TARGET)

# შესრულებადი ფაილის აწყობის წესი
$(TARGET): $(OBJ_FILES)
	@echo "Linking..."
	@mkdir -p $(@D)
	$(CC) $(OBJ_FILES) -o $@
	@echo "კომპილატორი 'მანუსკრიპტი' წარმატებით აიწყო!"

# .c ფაილების .o ფაილებად კომპილაციის წესი
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compiling $<..."
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(RUNTIME_DIR)/%.c
	@echo "Compiling runtime $<..."
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

# პროექტის გასუფთავების წესი
clean:
	@echo "Cleaning project..."
	rm -rf $(BUILD_DIR)
	@echo "Project cleaned."

# მაგალითის გაშვება
run: all
	@echo "Running example 'ცვლადები.მს'..."
	./$(TARGET) $(EXAMPLES_DIR)/ცვლადები.მს

.PHONY: all clean run