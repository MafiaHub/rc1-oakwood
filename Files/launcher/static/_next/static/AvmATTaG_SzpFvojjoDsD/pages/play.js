(window.webpackJsonp=window.webpackJsonp||[]).push([["1762"],{"0dXt":function(e,t,n){(window.__NEXT_P=window.__NEXT_P||[]).push(["/play",function(){var e=n("6B33");return{page:e.default||e}}])},"6B33":function(e,t,n){"use strict";n.r(t);var a=n("ln6h"),o=n.n(a),r=n("O40h"),u=n("q1tI"),c=n.n(u),l=n("b0oO"),i=n("iXVO"),d=n("y/r9"),s=function(){return window.oakwood&&oakwood.requestPlay()||console.info("mock call for requestPlay")};t.default=Object(d.a)(function(){return c.a.createElement(u.Fragment,null,c.a.createElement(l.a,null),c.a.createElement(i.b,null,"Ready to ",c.a.createElement(i.a,{onClick:s},"Play")))},Object(r.default)(o.a.mark(function e(){return o.a.wrap(function(e){for(;;)switch(e.prev=e.next){case 0:if(window.oakwood){e.next=2;break}return e.abrupt("return",console.info("mock call for checkForUpdate"));case 2:oakwood.checkForUpdate(),!0===oakwood.data.needsUpdate&&(Router.push("/update"),alert("we ok"));case 4:case"end":return e.stop()}},e)})))},"y/r9":function(e,t,n){"use strict";n.d(t,"a",function(){return d});var a=n("0iUn"),o=n("sLSF"),r=n("MI3g"),u=n("a7VT"),c=n("Tit0"),l=n("q1tI"),i=n.n(l),d=function(e,t){return function(n){function d(e){var t;return Object(a.default)(this,d),(t=Object(r.default)(this,Object(u.default)(d).call(this,e))).state={loading:!0,newProps:{}},t}return Object(c.default)(d,n),Object(o.default)(d,[{key:"componentDidMount",value:function(){var e=this;t().then(function(t){return e.setState({newProps:t,loading:0})})}},{key:"render",value:function(){return this.state.loading?i.a.createElement(l.Fragment,null):i.a.createElement(e,this.state.newProps)}}]),d}(l.Component)}}},[["0dXt","5d41","9da1"]]]);