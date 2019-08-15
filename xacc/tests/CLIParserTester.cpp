/*******************************************************************************
 * Copyright (c) 2019 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#include <gtest/gtest.h>

#include "CLIParser.hpp"
#include "RuntimeOptions.hpp"
using namespace xacc;

class Argv {
public:
  Argv(std::initializer_list<const char *> args)
      : m_argv(new char *[args.size()]), m_argc(args.size()) {
    int i = 0;
    auto iter = args.begin();
    while (iter != args.end()) {
      auto len = strlen(*iter) + 1;
      auto ptr = std::unique_ptr<char[]>(new char[len]);

      strcpy(ptr.get(), *iter);
      m_args.push_back(std::move(ptr));
      m_argv.get()[i] = m_args.back().get();

      ++iter;
      ++i;
    }
  }

  char **argv() const { return m_argv.get(); }

  int argc() const { return m_argc; }

private:
  std::vector<std::unique_ptr<char[]>> m_args;
  std::unique_ptr<char *[]> m_argv;
  int m_argc;
};

TEST(CLIParserTester, checkParse) {

  CLIParser parser;

  Argv argv({"tester", "--compiler", "quil", "--accelerator", "rigetti",
             "--test", "value", "--test2", "value2", "--logger-name", "xacc",
             "-I", "/path/to/include", "-I", "/path/to/other"});

  char **actual_argv = argv.argv();
  auto argc = argv.argc();

  parser.addOptions({{"test", "description"}, {"test2", "description"}});
  parser.parse(argc, actual_argv); //, nullptr);

  std::cout << "HELLO\n";
  auto options = *RuntimeOptions::instance();

  EXPECT_EQ(options["compiler"], "quil");
  EXPECT_EQ(options["accelerator"], "rigetti");
  EXPECT_EQ(options["test"], "value");
  EXPECT_EQ(options["test2"], "value2");

  EXPECT_EQ(options["logger-name"], "xacc");

  auto paths = parser.getIncludePaths();
  for (auto &p : paths) {
    std::cout << "CLParserTester Path: " << p << "\n";
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  return ret;
}
