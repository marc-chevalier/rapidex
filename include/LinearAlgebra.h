#pragma once

#include<vector>
#include<unordered_map>
#include<iostream>
#include<gmpxx.h>
#include<gmp.h>

class LinearAlgebra
{
public:
    template<typename T> static void scalarMultiplication(std::unordered_map<T, mpq_class>& result, mpq_class lhs, const std::unordered_map<T, mpq_class>& rhs) __attribute__((hot));
    template<typename T> static void scalarMultiplication(mpq_class lhs, std::unordered_map<T, mpq_class>& rhs) __attribute__((hot));
    template<typename T> static void sum(std::unordered_map<T, mpq_class>& result, const std::unordered_map<T, mpq_class>& lhs, const std::unordered_map<T, mpq_class>& rhs) __attribute__((hot));
    template<typename T> static void sum(std::unordered_map<T, mpq_class>& lhs, const std::unordered_map<T, mpq_class>& rhs) __attribute__((hot));
    template<typename T> static void minus(std::unordered_map<T, mpq_class>& result, const std::unordered_map<T, mpq_class>& lhs, const std::unordered_map<T, mpq_class>& rhs) __attribute__((hot));
    template<typename T> static void minus(std::unordered_map<T, mpq_class>& lhs, const std::unordered_map<T, mpq_class>& rhs) __attribute__((hot));
    template<typename T> static void substitution(std::unordered_map<T, mpq_class>& result, const std::unordered_map<T, mpq_class>& expression, const std::unordered_map<T, mpq_class>& substitutionExpression, T substitutedVariable) __attribute__((hot));
    template<typename T> static void substitution(std::unordered_map<T, mpq_class>& expression, const std::unordered_map<T, mpq_class>& substitutionExpression, T substitutedVariable) __attribute__((hot));
    template<typename T> static void simplify(std::unordered_map<T, mpq_class>& expression) __attribute__((hot));

    template<typename T> static std::unordered_map<T, mpq_class> substitution_old(const std::unordered_map<T, mpq_class>& expression, const std::unordered_map<T, mpq_class>& substitutionExpression, T substitutedVariable) __attribute__((hot));
};


template<typename T> void LinearAlgebra::scalarMultiplication(std::unordered_map<T, mpq_class>& result, mpq_class lhs, const std::unordered_map<T, mpq_class>& rhs)
{
    typedef typename std::unordered_map<T, mpq_class> unordered_map_t;
    typedef typename std::pair<T, mpq_class> pair_t;

    result.clear();

    if(lhs == 0)
        return result;

    for(const pair_t& terme : rhs)
        result[terme.first] = lhs * terme.second;
}

template<typename T> void LinearAlgebra::scalarMultiplication(mpq_class lhs, std::unordered_map<T, mpq_class>& rhs)
{
    typedef typename std::unordered_map<T, mpq_class> unordered_map_t;
    typedef typename std::pair<T, mpq_class> pair_t;

    if(lhs == 0)
        rhs.clear();
    else
        for(const pair_t& terme : rhs)
            rhs[terme.first] *= lhs;
}


template<typename T> void LinearAlgebra::sum(std::unordered_map<T, mpq_class>& result, const std::unordered_map<T, mpq_class>& lhs, const std::unordered_map<T, mpq_class>& rhs)
{
    typedef typename std::unordered_map<T, mpq_class> unordered_map_t;
    typedef typename std::pair<T, mpq_class> pair_t;

    result.clear();

    for(const pair_t& terme : lhs)
        result[terme.first] = terme.second;

    for(const pair_t& terme : rhs)
        if(result[terme.first] == -terme.second)
            result.erase(terme.first);
        else
            lhs[terme.first] += terme.second;
}

template<typename T> void LinearAlgebra::sum(std::unordered_map<T, mpq_class>& lhs, const std::unordered_map<T, mpq_class>& rhs)
{
    typedef typename std::unordered_map<T, mpq_class> unordered_map_t;
    typedef typename std::pair<T, mpq_class> pair_t;

    for(const pair_t& terme : rhs)
        if(lhs[terme.first] == -terme.second)
            lhs.erase(terme.first);
        else
            lhs[terme.first] += terme.second;
}


template<typename T> void LinearAlgebra::minus(std::unordered_map<T, mpq_class>& result, const std::unordered_map<T, mpq_class>& lhs, const std::unordered_map<T, mpq_class>& rhs)
{
    typedef typename std::unordered_map<T, mpq_class> unordered_map_t;
    typedef typename std::pair<T, mpq_class> pair_t;

    result.clear();

    for(const pair_t& terme : lhs)
        result[terme.first] = terme.second;

    for(const pair_t& terme : rhs)
        if(result[terme.first] == terme.second)
            result.erase(terme.first);
        else
            lhs[terme.first] -= terme.second;

    return result;
}

template<typename T> void LinearAlgebra::minus(std::unordered_map<T, mpq_class>& lhs, const std::unordered_map<T, mpq_class>& rhs)
{
    typedef typename std::unordered_map<T, mpq_class> unordered_map_t;
    typedef typename std::pair<T, mpq_class> pair_t;

    for(const pair_t& terme : rhs)
        if(lhs[terme.first] == terme.second)
            lhs.erase(terme.first);
        else
            lhs[terme.first] -= terme.second;
}

template<typename T> std::unordered_map<T, mpq_class> LinearAlgebra::substitution_old(const std::unordered_map<T, mpq_class>& expression, const std::unordered_map<T, mpq_class>& substitutionExpression, T substitutedVariable)
{
    typedef typename std::unordered_map<T, mpq_class> unordered_map_t;
    typedef typename std::pair<T, mpq_class> pair_t;

    if(expression.count(substitutedVariable)==0)
        return expression;

    unordered_map_t output = expression;
    output.erase(substitutedVariable);

    if(expression.at(substitutedVariable) == 0)
        return output;

    for(const pair_t& terme : substitutionExpression)
        if(output[terme.first] == -expression.at(substitutedVariable)*terme.second)
            output.erase(terme.first);
        else
            output[terme.first] += expression.at(substitutedVariable)*terme.second;

    return output;
}

template<typename T> void LinearAlgebra::substitution(std::unordered_map<T, mpq_class>& result, const std::unordered_map<T, mpq_class>& expression, const std::unordered_map<T, mpq_class>& substitutionExpression, T substitutedVariable)
{
    typedef typename std::unordered_map<T, mpq_class> unordered_map_t;
    typedef typename std::pair<T, mpq_class> pair_t;

    result = LinearAlgebra::substitution_old(expression, substitutionExpression, substitutedVariable);
    return;

    if(expression.count(substitutedVariable) == 0)
        return;

    if(expression.at(substitutedVariable) == 0)
        return;

    result = expression;
    result.erase(substitutedVariable);

    for(const pair_t& terme : substitutionExpression)

        if(result[terme.first] == -expression.at(substitutedVariable)*terme.second)
            result.erase(terme.first);
        else
            result[terme.first] += expression.at(substitutedVariable)*terme.second;
}

template<typename T> void LinearAlgebra::substitution(std::unordered_map<T, mpq_class>& expression, const std::unordered_map<T, mpq_class>& substitutionExpression, T substitutedVariable)
{
    typedef typename std::unordered_map<T, mpq_class> unordered_map_t;
    typedef typename std::pair<T, mpq_class> pair_t;

    if(expression.count(substitutedVariable) == 0)
        return;

    if(expression.at(substitutedVariable) == 0)
        return;

    for(const pair_t& terme : substitutionExpression)
        if(expression[terme.first] == -expression.at(substitutedVariable)*terme.second)
            expression.erase(terme.first);
        else
            expression[terme.first] += expression.at(substitutedVariable)*terme.second;

    expression.erase(substitutedVariable);
}

template<typename T> void LinearAlgebra::simplify(std::unordered_map<T, mpq_class>& expression)
{
    typedef typename std::unordered_map<T, mpq_class>::iterator it_t;
    std::vector<it_t> toErase;
    for(it_t it = expression.begin(); it != expression.end(); ++it)
        if(it->second == 0)
            toErase.push_back(it);

    for(const it_t& it : toErase)
        expression.erase(it);
}
