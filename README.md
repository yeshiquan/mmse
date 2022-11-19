# A Memory-based Search Engine(MMSE)

### 暂时处于开发阶段，可以运行单测看效果

```bash
cmake CmakeLists.txt
make
./mmse_test
```

#### 索引构建
```C++
void build_index() {
    IndexWriter index_writer;

    std::vector<std::string> doc_text = {
        "a b c d e f g a a a b c", // 0
        "a b c d e f g a b c", // 1
        "a b c d e f g a a a", // 2
        "c d e", // 3
        "c d e z", // 4
        "f z c", // 5
        "f g z", // 6
        "f g", // 7
        "f g", // 8
        "g" // 9
    };

    for (int i = 0; i < doc_text.size(); ++i) {
        auto doc = std::make_shared<Document>(i);
        doc->add_field(new TextField(0, doc_text[i], YES));
        doc->add_field(new NumberField(1, i*2, YES));
        doc->add_field(new StringField(2, "hello", YES));
        index_writer.add_document(doc);
    }
    index_writer.build();
}
```

#### 检索
```C++
Query* query1 = new TermQuery(Term("content", "f"));
Query* query2 = new TermQuery(Term("content", "c"));
BooleanQuery* query = new BooleanQuery;
query->add(query1, Occur::MUST);
query->add(query2, Occur::MUST_NOT);
Weight* weight = query->create_weight();
Scorer* scorer = weight->make_scorer();

std::vector<DocId> result;
std::vector<DocId> expect_result{6,7,8};
DocId doc = scorer->next_doc();
while (doc != NO_MORE_DOCS) {
    std::cout << "Query a doc -> " << doc << std::endl;
    result.push_back(doc);
    doc = scorer->next_doc();
}
```