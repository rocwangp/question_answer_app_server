#pragma once


#include "StringUtil.h"
#include <unordered_map>

using std::string;
using std::unordered_map;

class Question
{
public:
    Question() {}
    Question(int questionId,               const string& question,
             const string& questionDetail, const string& date, 
             const string& userId,         const string& answerIds)
        : questionId_(questionId),
          question_(question),
          questionDetail_(questionDetail),
          date_(date),
          userId_(userId),
          answerIds_(answerIds)
    {}

    ~Question() {}

    void setQuestionId(int questionId) { questionId_ = questionId; }
    void setQuestion(const string& question) { question_ = question; }
    void setQuestionDetail(const string& questionDetail) { questionDetail_ = questionDetail; }
    void setDate(const string& date) { date_ = date; }
    void setUserId(const string& userId) { userId_ = userId; }
    void setAnswerIds(const string& answerIds) { answerIds_ = answerIds; }

    int questionId() const { return questionId_; }
    string question() const { return question_; }
    string questionDetail() const { return questionDetail_; }
    string date() const { return date_; }
    string userId() const { return userId_; }
    string answerIds() const { return answerIds_;}

    bool isValid() const
    {
        return !question_.empty();
    }
    unordered_map<string, string> toMap()
    {
        unordered_map<string, string> questionInfo;
        questionInfo["questionId"] = StringUtil::toString(questionId_);
        questionInfo["question"] = question_;
        questionInfo["questionDetail"] = questionDetail_;
        questionInfo["date"] = date_;
        questionInfo["userId"] = userId_;
        questionInfo["answerIds"] = answerIds_;
        return questionInfo;
    }
private:
    int questionId_;
    string question_;
    string questionDetail_;
    string date_;
    string userId_;
    string answerIds_;
};


class Answer
{
public:
    Answer() {}
    Answer(int answerId, const string& answer,
           const string& date,     const string& userId,
           const string& commentIdStr)
        : answerId_(answerId),
          answer_(answer),
          date_(date),
          userId_(userId),
          commentIds_(commentIdStr)
    {}
    ~Answer() {}

    void setAnswerId(int answerId) { answerId_ = answerId; }
    void setAnswer(const string& answer) { answer_= answer; }
    void setDate(const string& date) { date_ = date; }
    void setUserId(const string& userId) { userId_ = userId; }
    void commentIds(const string& commentIds) { commentIds_ = commentIds; }

    int answerId() const { return answerId_; }
    string answer() const { return answer_; }
    string date() const { return date_; }
    string userId() const { return userId_; }
    string commentIdStr() const { return commentIds_; }

    bool isValid() const
    {
        return !answer_.empty();
    }
    unordered_map<string, string> toMap()
    {
        unordered_map<string, string> answerInfo;
        answerInfo["answerId"] = StringUtil::toString(answerId_);
        answerInfo["answer"] = answer_;
        answerInfo["date"] = date_;
        answerInfo["userId"] = userId_;
        answerInfo["commentId"] = commentIds_;
        return answerInfo;
    }
private:
    int answerId_;
    string answer_;
    string date_;
    string userId_;
    string commentIds_;
};


class Comment
{
public:
    Comment() {}
    Comment(int commentId,           const string& comment,
            const string& date,      const string& userId)
        : commentId_(commentId),
          comment_(comment),
          date_(date),
          userId_(userId)
    {}
    ~Comment() {}

    void setCommentId(int commentId) { commentId_ = commentId; }
    void setComment(const string& comment) { comment_ = comment; }
    void setDate(const string& date) { date_ = date; }
    void setUserId(const string& userId) { userId_ = userId; }

    unordered_map<string, string> toMap()
    {
        unordered_map<string, string> commentInfo;
        commentInfo["commentId"] = StringUtil::toString(commentId_);
        commentInfo["comment"] = comment_;
        commentInfo["date"] = date_;
        commentInfo["userId"] = userId_;
        return commentInfo;
    }
private:
    int commentId_;
    string comment_;
    string date_;
    string userId_;
};
