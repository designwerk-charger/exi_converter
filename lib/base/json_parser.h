#pragma once

#include <cstdint>
#include <cstdio>
#include <string>
#include <sstream>
#include <utility>
#include <vector>
#include <memory>
#include <map>

class JValue;
class JArray;
class JObject;

class JItemInterface {
 public:
    [[nodiscard]] virtual const std::string & get_value() const {throw std::runtime_error("Conversion to JValue failed!");}
    [[nodiscard]] virtual const std::vector<std::shared_ptr<JItemInterface>> & get_array() const {throw std::runtime_error("Conversion to JArray failed!");}
    virtual const JObject & to_object() {throw std::runtime_error("Conversion to JObject failed!");}
    virtual std::shared_ptr<JObject> get_object(const std::string & key) {throw std::runtime_error("Conversion to JObject failed!");}
    virtual bool is_empty() {return false;}
};


class JEmpty: public JItemInterface {
 public:
    bool is_empty() override {return true;}
};

class JValue: public JItemInterface {
 public:
    explicit JValue(std::string content) : content(std::move(content)) {}
    [[nodiscard]] const std::string & get_value() const override {return content;}

 private:
    std::string content;
};

class JArray: public JItemInterface {
 public:
    JArray() : content({}) {}
    [[nodiscard]] const std::vector<std::shared_ptr<JItemInterface>> & get_array() const override {return content;}
    void append(std::shared_ptr<JItemInterface> item) {content.emplace_back(std::move(item));}
    bool is_empty() override {return content.empty();}

 private:
    std::vector<std::shared_ptr<JItemInterface>> content;
};

class JObject: public JItemInterface {
 public:
    JObject() : content({}), iterator(content.end()) {}

    const JObject & to_object() override {return *this;}

    bool exists(const std::string & key) {
        return content.find(key) != content.end();
    }

    std::shared_ptr<JObject> get_object(const std::string & key) override {
        return std::dynamic_pointer_cast<JObject>(content[key]);
    }

    void append(std::pair<std::string, std::shared_ptr<JItemInterface>> item) {content.insert(std::move(item));}

    const std::shared_ptr<JItemInterface> & operator [] (const std::string & key) const {return content.at(key);}

    [[nodiscard]] size_t size() const {return content.size();}

    const std::string & get_next_key_name() {
        if (iterator == content.end()) {
            iterator = content.begin();
        }
        return iterator++->first;
    }

 private:
    std::map<std::string, std::shared_ptr<JItemInterface>> content;
    std::map<std::string, std::shared_ptr<JItemInterface>>::iterator iterator;
};



class JsonParser {
 public:
    explicit JsonParser(const std::string & input_data);
    JsonParser();
    virtual ~JsonParser() = default;  // LCOV_EXCL_LINE

    static std::shared_ptr<JObject> parse(const std::string & json_str);

 private:
    std::shared_ptr<JObject> parsed_json;
};
