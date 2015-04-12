#pragma once

class LinearAlgebra
{
public:
    template<typename T> static std::map<T, mpq_class> scalarMultiplication(mpq_class lhs, const std::map<T, mpq_class>& rhs)
    {
        typedef typename std::map<T, mpq_class> map_t;
        typedef typename std::pair<T, mpq_class> pair_t;
        map_t output;

        for(pair_t terme : rhs)
            output[terme.first] = lhs * terme.second;

        simplify(output);

        return output;
    }
    template<typename T> static std::map<T, mpq_class> sum(const std::map<T, mpq_class>& lhs, const std::map<T, mpq_class>& rhs)
    {
        typedef typename std::map<T, mpq_class> map_t;
        typedef typename std::pair<T, mpq_class> pair_t;
        map_t output;

        for(pair_t terme : lhs)
            output[terme.first] = terme.second;

        for(pair_t terme : rhs)
            output[terme.first] += terme.second;

        simplify(output);

        return output;
    }
    template<typename T> static std::map<T, mpq_class> minus(const std::map<T, mpq_class>& lhs, const std::map<T, mpq_class>& rhs)
    {
        typedef typename std::map<T, mpq_class> map_t;
        typedef typename std::pair<T, mpq_class> pair_t;
        map_t output;

        for(pair_t terme : lhs)
            output[terme.first] = terme.second;

        for(pair_t terme : rhs)
            output[terme.first] -= terme.second;

        simplify(output);

        return output;
    }
    template<typename T> static std::map<T, mpq_class> substitution(const std::map<T, mpq_class>& expression, const std::map<T, mpq_class>& substitutionExpression, T substitutedVariable)
    {
        typedef typename std::map<T, mpq_class> map_t;

        if(expression.count(substitutedVariable)==0)
            return expression;

        map_t output = expression;
        output.erase(substitutedVariable);

        output = sum(output, scalarMultiplication(expression.at(substitutedVariable), substitutionExpression));
        simplify(output);
        return output;
    }
    template<typename T> static void simplify(std::map<T, mpq_class>& expression)
    {
        typedef typename std::map<T, mpq_class>::iterator it_t;
        std::vector<it_t> toErase;
        for(it_t it = expression.begin(); it != expression.end(); ++it)
            if(it->second == 0)
                toErase.push_back(it);

        for(it_t it : toErase)
            expression.erase(it);
    }
};
