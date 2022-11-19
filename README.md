# A Memory-based Search Engine(MMSE)

### 编译 & 运行

```bash
cmake CmakeLists.txt
make
./mmse_test
```

#### 索引构建
```C++
IndexWriter index_writer;

std::vector<std::string> doc_text = {
    "a b c d e f g a a a b c", // doc_id: 0
    "a b c d e f g a b c", // doc_id: 1
    "a b c d e f g a a a", // doc_id: 2
    "c d e", // doc_id: 3
    "c d e z", // doc_id: 4
    "f z c", // doc_id: 5
    "f g z", // doc_id: 6
    "f g", // doc_id: 7
    "f g", // doc_id: 8
    "g" // doc_id: 9
};

for (int i = 0; i < doc_text.size(); ++i) {
    auto doc = std::make_shared<Document>(i);
    doc->add_field(new TextField(0, doc_text[i], FieldStoreType::YES));
    doc->add_field(new NumberField(1, i*2, FieldStoreType::YES));
    doc->add_field(new StringField(2, "hello", FieldStoreType::YES));
    index_writer.add_document(doc);
}
index_writer.build();
```

#### 检索
```C++
auto query1 = mmse::make<TermQuery>(Term("content", "f"));
auto query2 = mmse::make<TermQuery>(Term("content", "c"));
auto query = mmse::make<BooleanQuery>();
query->add(query1, Occur::MUST);
query->add(query2, Occur::MUST_NOT);
WeightPtr weight = query->create_weight();
ScorerPtr scorer = weight->make_scorer();

std::vector<DocId> result;
DocId doc = scorer->next_doc();
while (doc != NO_MORE_DOCS) {
    std::cout << "Query a doc -> " << doc << std::endl;
    result.push_back(doc);
    doc = scorer->next_doc();
}
```
