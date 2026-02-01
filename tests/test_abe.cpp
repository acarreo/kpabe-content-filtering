#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <math.h>
#include <gtest/gtest.h>
#include <abe_lsss/abe_lsss.h>

#include "kpabe.hpp"


using namespace std;

#define TEST_MSG_LEN 32

#define COMMA ','
#define TEST_DESCRIPTION(desc) RecordProperty("description", desc)
#define TESTSUITE_DESCRIPTION(desc)                                            \
  ::testing::Test::RecordProperty("description", desc)


string createAttribute(int i) {
  stringstream ss;
  ss << "Attr" << i;
  return ss.str();
}

bool getOpenABEAttributeList(int max, vector<string> &attrList)
{
  if (max <= 0) return false;

  // reset
  attrList.clear();
  for (int i = 0; i <= max; i++) {
    attrList.push_back(createAttribute(i));
  }

  return true;
}


// returns a right-sided skewed policy tree
string getOpenABEPolicyString(int max)
{
  string policystr;
  if (max >= 2) {
    policystr = "(" + createAttribute(0) + " and " + createAttribute(1) + ")";
  } else if (max == 1) {
    policystr = createAttribute(0);
  }

  for (int i = 2; i <= max; i++) {
    policystr = "(" + policystr + " and " + createAttribute(i) + ")";
  }

  return policystr;
}


namespace {

class Input {
public:
  Input(const string url_input, const string enc_input,
        const string key_input, vector<string> wl_, vector<string> bl_,
        bool expect_pass_, bool verbose_ = false)
  {
    attributes = enc_input;
    policy = key_input;
    url = url_input;
    white_list = wl_;
    black_list = bl_;
    expect_pass = expect_pass_;
    verbose = verbose_;
  }
  ~Input(){};

  string url, attributes, policy, policy_str;
  vector<string> white_list, black_list, attr_list;
  bool verbose, expect_pass;
};

class SecurityForSchemeTest : public ::testing::TestWithParam<Input> {
protected:
  virtual void SetUp() {
    getRandomBytes(plaintext, TEST_MSG_LEN);
    MPK = "testMPK";
    MSK = "testMSK";
    AUTH1MPK = "auth1", AUTH1MSK = "auth1MSK";
    AUTH2MPK = "auth2", AUTH2MSK = "auth2MSK";
  }

  ByteString mpkBlob, mskBlob, dkBlob, ctBlob;
  ByteString plaintext, plaintext1;
  string MPK, MSK, AUTH1MPK, AUTH1MSK, AUTH2MPK, AUTH2MSK;
};

class CCASecurityForKEMTest : public SecurityForSchemeTest {};
class CCASecurityForSchemeTest : public SecurityForSchemeTest {};

/* Unit tests for CPA scheme contexts */
TEST_P(SecurityForSchemeTest, testWorkingExamples) {
  Input input = GetParam();
  TEST_DESCRIPTION("Testing CPA-secure KP-ABE based DPVS scheme with Key: '" +
                    input.policy + "' and Enc: '" + input.attributes +
                    "' for the URL: '" + input.url + "'");

  cout << "-----------> Testing CPA security for KP-ABE based DPVS scheme for the whitelist of size: "
       << input.white_list.size() << " and for the blacklist of size: " << input.black_list.size() << endl;


  // Generate a set of parameters by an ABE authority - Setup Algorithm
  KPABE_DPVS kpabe; ASSERT_TRUE(kpabe.setup());

  auto mpk = kpabe.get_public_key();
  auto msk = kpabe.get_master_key();

  // Export master public and secret keys to bytes - Serialize
  // Check size of keys - Size of Blob should equal to expected size
  mpk.serialize(mpkBlob); ASSERT_TRUE(mpkBlob.size() == mpk.getSizeInBytes());
  msk.serialize(mskBlob); ASSERT_TRUE(mskBlob.size() == msk.getSizeInBytes());

  // Load master public and secret keys from bytes - Deserialize
  // Check that the loaded keys are equal to the origal exported keys
  KPABE_DPVS_PUBLIC_KEY mpk2;
  KPABE_DPVS_MASTER_KEY msk2;

  mpk2.deserialize(mpkBlob); ASSERT_TRUE(mpk == mpk2);
  msk2.deserialize(mskBlob); ASSERT_TRUE(msk == msk2);

  // Encrypt under the specified functional input - Attributes list
  // Here we check just if the attributes list is well formed
  unique_ptr<OpenABEFunctionInput> encInput = createAttributeList(input.attributes);
  ASSERT_FALSE(encInput == nullptr);

  unique_ptr<OpenABEFunctionInput> keyInput = createPolicyTree(input.policy);
  ASSERT_FALSE(keyInput == nullptr);

  auto dk = kpabe.keygen(input.policy, input.white_list, input.black_list);
  bool is_dk_ok = false;
  if (dk) {
    is_dk_ok = true;
  }
  ASSERT_TRUE(is_dk_ok);
  dk->serialize(dkBlob); ASSERT_TRUE(dkBlob.size() == dk->getSizeInBytes());

  KPABE_DPVS_DECRYPTION_KEY dk2;
  dk2.deserialize(dkBlob); ASSERT_TRUE(*dk == dk2);


  // Encryption & Decryption
  uint8_t sym_key_1[RLC_MD_LEN];
  uint8_t sym_key_2[RLC_MD_LEN];

  KPABE_DPVS_CIPHERTEXT ciphertext(input.attributes, input.url);
  ASSERT_TRUE(ciphertext.encrypt(sym_key_1, mpk));


  // Decrypt the ciphertext with multiple keys
  ASSERT_TRUE(ciphertext.decrypt(sym_key_2, *dk) == input.expect_pass);
  if (input.expect_pass) {
    ASSERT_TRUE(memcmp(sym_key_1, sym_key_2, RLC_MD_LEN) == 0);
  } else {
    ASSERT_FALSE(memcmp(sym_key_1, sym_key_2, RLC_MD_LEN) == 0);
  }

  if (input.verbose) {
    ByteString sym_key_1_Blob, sym_key_2_Blob;
    sym_key_1_Blob.appendArray(sym_key_1, RLC_MD_LEN);
    sym_key_2_Blob.appendArray(sym_key_2, RLC_MD_LEN);
    cout << "Symmetric key generated: " << sym_key_1_Blob.toHex() << endl;
    cout << "Enc Attributes Input used: " << input.attributes << endl;
    cout << "Key policy Input used: " << input.policy << endl;
    cout << "Symmetric key recovered: " << sym_key_1_Blob.toHex() << endl;
    cout << "Test expected to pass: " << (input.expect_pass ? "true" : "false") << endl;
  }
}
} // namespace


//  Input(const string url_input, const string enc_input,
//        const string key_input, vector<string> wl_, vector<string> bl_,
//        bool expect_pass_, bool verbose_ = false)

vector<string> bl({"www.facebook.com", "www.twitter.com", "www.instagram.com", "www.youtube.com", "www.tiktok.com"});
vector<string> wl({"www.example.com", "www.google.com", "www.amazon.com", "www.nytimes.com", "www.wikipedia.org"});

INSTANTIATE_TEST_SUITE_P(
    ABETest1, SecurityForSchemeTest,
    ::testing::Values(
        // "==================== POLICY SATISFIED – DECRYPTION OK ===================="
        Input("www.perdu.com", "A1|A2|A4|A5",
              "A5 and ((A1 and A2) or (A3 and A4))", wl, bl, true),
        Input("www.computer-science.com", "A1|A4",
              "(A1 or A2 or A3) and (A4 or A5)", wl, bl, true),
        // "================= POLICY NOT SATISFIED – DECRYPTION FAIL ================="
        Input("www.github.com", "A2|A3|A7|A9",
              "(A1 or A2 or A3) and (A4 or A5)", wl, bl, false),
        Input("www.microsoft.com", "A1|A4",
              "(A1 and A2 and A3) or (A4 and A5 and A6)", wl, bl, false),
        // "======================= URL WHITELISTED – ACCESS OK ======================"
        Input("www.wikipedia.org", "A1|A2|A4|A5",
              "A5 and ((A1 and A2) or (A3 and A4))", wl, bl, true),
        Input("www.nytimes.com", "A1|A3",
              "(A1 or A2) and (A3 or A4)", wl, bl, true),
        // "======================= URL BLACKLISTED – ACCESS DENIED =================="
        Input("www.youtube.com", "A1|A4|A6|A8",
              "(A1 or A2 or A3) and (A4 or A5)", wl, bl, false),
        Input("www.twitter.com", "A2|A3",
              "(A1 and A2) and (A3 or A4)", wl, bl, false)
));

 
#if 0
INSTANTIATE_TEST_SUITE_P(
    ABETest2, SecurityForSchemeTest,
    ::testing::Values(Input(,
                            "Alice and Date = May 1-10, 2016",
                            "Alice|Date=May 5, 2016", true),
                      Input(,
                            "Date = May 1-10, 2016 and (Alice or Bob)",
                            "Bob|Date=May 8, 2016", true),
                      Input(,
                            "((Alice or Bob) and Date = May 1-10, 2016)",
                            "Bob|Eve|Date=May 12, 2016", false)));
#endif

#if 0
class SatInput {
public:
  SatInput(const string policy_str, const string attr_list, bool expect_pass) {
    policy_str_ = policy_str;
    attr_list_ = attr_list;
    expect_pass_ = expect_pass;
    verbose_   = false;
  }
  ~SatInput() {};
  string policy_str_, attr_list_;
  bool verbose_, expect_pass_;
};

class CheckIfSatisfiedTests : public ::testing::TestWithParam<SatInput> {
  protected:
    virtual void SetUp() {}
};

TEST_P(CheckIfSatisfiedTests, testWorkingExamples) {
  SatInput input = GetParam();
  TEST_DESCRIPTION("Checking sat for: '" + input.policy_str_ + "' sat by '" + input.attr_list_ + "'");

  unique_ptr<OpenABEPolicy> policy = createPolicyTree(input.policy_str_);
  ASSERT_TRUE(policy != nullptr);
  unique_ptr<OpenABEAttributeList> attr_list = createAttributeList(input.attr_list_);
  ASSERT_TRUE(attr_list != nullptr);
  pair<bool,int> res = checkIfSatisfied(policy.get(), attr_list.get());
  if (input.expect_pass_) {
    ASSERT_TRUE(res.first);
  } else {
    ASSERT_FALSE(res.first);
  }
}

INSTANTIATE_TEST_SUITE_P(CheckSat1, CheckIfSatisfiedTests,
  ::testing::Values(
  // test standard or/and type policy combos
  SatInput("((Alice or Bob) or David)", "Bob", true),
  SatInput("((Alice and Bob) or David)", "Bob|David", true),
  SatInput("(Alice and (Bob or David))", "Bob|David", false),
  SatInput("(Alice and (Bob and David))", "Alice|Bob|David", true),
  SatInput("((Alice or Bob) and David)", "Bob|David", true),
  SatInput("((Alice or Bob) and David)", "Alice|Charlie", false),
  SatInput("((Alice or Bob) and David)", "Alice|David", true),
  SatInput("(David or Charlie)", "Alice|Bob", false),
  SatInput("Bar", "Alice|Bob", false),
  SatInput("Alice", "Alice|Bob", true),
  SatInput("Foor", "Bar", false),
  // test uids
  SatInput("((Alice and Bob) or uid:567abcdef)", "uid:567abcdef|Bob", true),
  SatInput("((Alice or Bob) and uid:567abcdef)", "uid:567abcdef|Bob", true),
  // test integer range
  SatInput("(Floor in (2-5) and Alice)", "Alice|Floor=3", true),
  SatInput("(Floor in (2-5) and Alice)", "Alice|Floor=7", false),
  // test dates and date ranges
  SatInput("(David or Date = January 1-31, 2015)", "David|Bob", true),
  SatInput("(David or Date = January 1-31, 2015)", "Date=January 27, 2015|Bob", true),
  SatInput("(David or Date = January 1-31, 2015)", "Date=March 17, 2015|Bob", false),
  SatInput("Date > January 1, 1971", "Date = January 1, 2010", true),
  SatInput("Date >= January 1, 1971", "Date = January 1, 1971", true),
  SatInput("Date <= January 1, 1971", "Date = January 1, 1975", false),
  SatInput("Date < January 1, 1971", "Date = December 1, 2000", false)
));
#endif


int main(int argc, char **argv) {
  int rc;

  InitializeOpenABE();

  ::testing::InitGoogleTest(&argc, argv);
  rc = RUN_ALL_TESTS();

  ShutdownOpenABE();

  return rc;
}
